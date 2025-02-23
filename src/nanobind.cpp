#include <Common.hh>

#include <game/system/KPadDirector.hh>
#include <game/system/RaceConfig.hh>

#include <egg/math/Vector.hh>
#include <egg/math/Quat.hh>
#include <egg/core/ExpHeap.hh>
#include <egg/core/SceneManager.hh>

#include <game/kart/KartObjectManager.hh>
#include <game/kart/KartDynamics.hh>
#include <game/kart/KartState.hh>
#include <game/kart/KartMove.hh>
#include <game/system/RaceManager.hh>
#include <game/item/ItemDirector.hh>

#include <host/SceneCreatorDynamic.hh>

#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/string.h>
#include <nanobind/ndarray.h>

#include <sstream>

#include "gfx/MkwVis.hpp"

//namespace py = pybind11;
namespace nb = nanobind;
using namespace nb::literals;

using namespace System;
using namespace Host;


#if defined(__arm64__) || defined(__aarch64__)
static void FlushDenormalsToZero() {
    uint64_t fpcr;
    asm("mrs %0,   fpcr" : "=r"(fpcr));
    asm("msr fpcr, %0" ::"r"(fpcr | (1 << 24)));
}
#elif defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>

static void FlushDenormalsToZero() {
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
}
#endif

#if defined(__arm64__) || defined(__aarch64__)
static void KeepDenormals() {
    uint64_t fpcr;
    asm("mrs %0,   fpcr" : "=r"(fpcr));
    asm("msr fpcr, %0" ::"r"(fpcr & ~(1 << 24)));
}
#elif defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>

static void KeepDenormals() {
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_OFF);
}
#endif

static void *s_memorySpace = nullptr;
static EGG::Heap *s_rootHeap = nullptr;
static void InitMemory() {
    constexpr size_t MEMORY_SPACE_SIZE = 0x1000000;
    Abstract::Memory::MEMiHeapHead::OptFlag opt;
    opt.setBit(Abstract::Memory::MEMiHeapHead::eOptFlag::ZeroFillAlloc);

#ifdef BUILD_DEBUG
    opt.setBit(Abstract::Memory::MEMiHeapHead::eOptFlag::DebugFillAlloc);
#endif

    s_memorySpace = malloc(MEMORY_SPACE_SIZE);
    s_rootHeap = EGG::ExpHeap::create(s_memorySpace, MEMORY_SPACE_SIZE, opt);
    s_rootHeap->setName("EGGRoot");
    s_rootHeap->becomeCurrentHeap();

    EGG::SceneManager::SetRootHeap(s_rootHeap);
}
struct HeapManager {
    HeapManager() {
        InitMemory();
    }
    ~HeapManager() {
        //free(s_memorySpace);
        //free(s_rootHeap);
    }
};

class KHostSystem {
  EGG::SceneManager *m_sceneMgr;
  HeapManager heapMgr;
  u16 buttonsPrev;
  bool inDrift;

  MkwVis* mkwVis;

public:
  ~KHostSystem() { mkwVis->destroyWindow(); }
  void configure(Course course, Character character, Vehicle vehicle, bool isAuto);
  void init();
  bool setInput(u16 buttons, u8 stickXRaw, u8 stickYRaw, Trick trick);
  void calc();
  void reset();
  const Kart::KartObjectProxy& kartObjectProxy();
  const System::RaceManager& raceManager();
  const Item::ItemDirector& itemDirector();
  f32 raceCompletion();
};

void KHostSystem::configure(Course course, Character character, Vehicle vehicle, bool isAuto) {
    System::RaceConfig::RegisterInitCallback([course, character, vehicle, isAuto](System::RaceConfig *config, void *arg) {
        config->raceScenario().course = course;

        System::RaceConfig::Player& player = config->raceScenario().players[0];
        player.type = System::RaceConfig::Player::Type::Local;
        player.character = character;
        player.vehicle = vehicle;
        player.driftIsAuto = isAuto;
    }, nullptr);
}

void KHostSystem::init() {
    FlushDenormalsToZero();

    auto *sceneCreator = new SceneCreatorDynamic;
    m_sceneMgr = new EGG::SceneManager(sceneCreator);

    m_sceneMgr->changeScene(0);

    buttonsPrev = 0;
    inDrift = false;

    KeepDenormals();

    mkwVis = new MkwVis(Field::CourseColMgr::Instance()->data());
    mkwVis->createWindow(1200, 800);
    mkwVis->load();
}

class ButtonInput {
public:
    static constexpr int ACCELERATE = 0x1;
    static constexpr int BRAKE = 0x2;
    static constexpr int ITEM = 0x4;
    static constexpr int DRIFT = 0x8;

    static constexpr int KPAD_BUTTON_A = 0x1;
    static constexpr int KPAD_BUTTON_B = 0x2;
    static constexpr int KPAD_BUTTON_ITEM = 0x4;
};

unsigned short encode_buttons(const std::vector<int>& buttons) {
    unsigned short encoded = 0;
    for (int button : buttons) {
        encoded |= button;  // Combine the button values
    }
    return encoded;
}

bool KHostSystem::setInput(u16 buttons, u8 stickXRaw, u8 stickYRaw, Trick trick) {
    u16 buttonsTrig = (buttons ^ buttonsPrev) & buttons;

    u16 faceBtn = 0;
    // face button logic
    bool aBtnHeld = (buttons & ButtonInput::KPAD_BUTTON_A) != 0;
    if (aBtnHeld) {
        faceBtn = faceBtn | ButtonInput::ACCELERATE;
    }
    bool bBtnHeld = (buttons & ButtonInput::KPAD_BUTTON_B) != 0;
    if (bBtnHeld) {
        faceBtn = faceBtn | ButtonInput::BRAKE;
    }
    bool itemBtnHeld = (buttons & ButtonInput::KPAD_BUTTON_ITEM) != 0;
    if (itemBtnHeld) {
        faceBtn = faceBtn | ButtonInput::ITEM;
    }
    if (!bBtnHeld || !aBtnHeld) {
        inDrift = false;
    }
    else if ((buttonsTrig & ButtonInput::KPAD_BUTTON_B) != 0) {
        inDrift = true;
    }
    if (inDrift) {
        faceBtn = faceBtn | ButtonInput::DRIFT;
    }
    buttonsPrev = buttons;

    return KPadDirector::Instance()->hostController()->setInputsRawStick(faceBtn, stickXRaw, stickYRaw, trick);
}

void KHostSystem::calc() {
    FlushDenormalsToZero();

    m_sceneMgr->calc();

    KeepDenormals();

    mkwVis->update();
    const Kart::KartObjectProxy& proxy = kartObjectProxy();
    mkwVis->setPose(proxy.pos(), proxy.mainRot());
    mkwVis->draw();
}

void KHostSystem::reset() {
    FlushDenormalsToZero();

    m_sceneMgr->destroyScene(m_sceneMgr->currentScene());
    m_sceneMgr->createScene(2, m_sceneMgr->currentScene());

    buttonsPrev = 0;
    inDrift = false;

    KeepDenormals();
}

const Kart::KartObjectProxy& KHostSystem::kartObjectProxy() {
    return *Kart::KartObjectManager::Instance()->object(0);
}

const System::RaceManager& KHostSystem::raceManager() {
    return *System::RaceManager::Instance();
}

const Item::ItemDirector& KHostSystem::itemDirector() {
    return *Item::ItemDirector::Instance();
}

f32 KHostSystem::raceCompletion() {
    const auto &player = System::RaceManager::Instance()->player();
    return player.raceCompletion();
}

NB_MODULE(pynoko, m) {
    nb::enum_<Trick>(m, "Trick")
        .value("NoTrick", Trick::None)
        .value("Up", Trick::Up)
        .value("Down", Trick::Down)
        .value("Left", Trick::Left)
        .value("Right", Trick::Right);

    nb::enum_<Course>(m, "Course")
        .value("Mario_Circuit", Course::Mario_Circuit)
        .value("Moo_Moo_Meadows", Course::Moo_Moo_Meadows)
        .value("Mushroom_Gorge", Course::Mushroom_Gorge)
        .value("Grumble_Volcano", Course::Grumble_Volcano)
        .value("Toads_Factory", Course::Toads_Factory)
        .value("Coconut_Mall", Course::Coconut_Mall)
        .value("DK_Summit", Course::DK_Summit)
        .value("Wario_Gold_Mine", Course::Wario_Gold_Mine)
        .value("Luigi_Circuit", Course::Luigi_Circuit)
        .value("Daisy_Circuit", Course::Daisy_Circuit)
        .value("Moonview_Highway", Course::Moonview_Highway)
        .value("Maple_Treeway", Course::Maple_Treeway)
        .value("Bowsers_Castle", Course::Bowsers_Castle)
        .value("Rainbow_Road", Course::Rainbow_Road)
        .value("Dry_Dry_Ruins", Course::Dry_Dry_Ruins)
        .value("Koopa_Cape", Course::Koopa_Cape)
        .value("GCN_Peach_Beach", Course::GCN_Peach_Beach)
        .value("GCN_Mario_Circuit", Course::GCN_Mario_Circuit)
        .value("GCN_Waluigi_Stadium", Course::GCN_Waluigi_Stadium)
        .value("GCN_DK_Mountain", Course::GCN_DK_Mountain)
        .value("DS_Yoshi_Falls", Course::DS_Yoshi_Falls)
        .value("DS_Desert_Hills", Course::DS_Desert_Hills)
        .value("DS_Peach_Gardens", Course::DS_Peach_Gardens)
        .value("DS_Delfino_Square", Course::DS_Delfino_Square)
        .value("SNES_Mario_Circuit_3", Course::SNES_Mario_Circuit_3)
        .value("SNES_Ghost_Valley_2", Course::SNES_Ghost_Valley_2)
        .value("N64_Mario_Raceway", Course::N64_Mario_Raceway)
        .value("N64_Sherbet_Land", Course::N64_Sherbet_Land)
        .value("N64_Bowsers_Castle", Course::N64_Bowsers_Castle)
        .value("N64_DKs_Jungle_Parkway", Course::N64_DKs_Jungle_Parkway)
        .value("GBA_Bowser_Castle_3", Course::GBA_Bowser_Castle_3)
        .value("GBA_Shy_Guy_Beach", Course::GBA_Shy_Guy_Beach)
        .value("Delfino_Pier", Course::Delfino_Pier)
        .value("Block_Plaza", Course::Block_Plaza)
        .value("Chain_Chomp_Roulette", Course::Chain_Chomp_Roulette)
        .value("Funky_Stadium", Course::Funky_Stadium)
        .value("Thwomp_Desert", Course::Thwomp_Desert)
        .value("GCN_Cookie_Land", Course::GCN_Cookie_Land)
        .value("DS_Twilight_House", Course::DS_Twilight_House)
        .value("SNES_Battle_Course_4", Course::SNES_Battle_Course_4)
        .value("GBA_Battle_Course_3", Course::GBA_Battle_Course_3)
        .value("N64_Skyscraper", Course::N64_Skyscraper)
        .value("Galaxy_Colosseum", Course::Galaxy_Colosseum)
        .value("Win_Demo", Course::Win_Demo)
        .value("Lose_Demo", Course::Lose_Demo)
        .value("Draw_Demo", Course::Draw_Demo)
        .value("Ending_Demo", Course::Ending_Demo);

    nb::enum_<Vehicle>(m, "Vehicle")
        .value("Standard_Kart_S", Vehicle::Standard_Kart_S)
        .value("Standard_Kart_M", Vehicle::Standard_Kart_M)
        .value("Standard_Kart_L", Vehicle::Standard_Kart_L)
        .value("Baby_Booster", Vehicle::Baby_Booster)
        .value("Classic_Dragster", Vehicle::Classic_Dragster)
        .value("Offroader", Vehicle::Offroader)
        .value("Mini_Beast", Vehicle::Mini_Beast)
        .value("Wild_Wing", Vehicle::Wild_Wing)
        .value("Flame_Flyer", Vehicle::Flame_Flyer)
        .value("Cheep_Charger", Vehicle::Cheep_Charger)
        .value("Super_Blooper", Vehicle::Super_Blooper)
        .value("Piranha_Prowler", Vehicle::Piranha_Prowler)
        .value("Tiny_Titan", Vehicle::Tiny_Titan)
        .value("Daytripper", Vehicle::Daytripper)
        .value("Jetsetter", Vehicle::Jetsetter)
        .value("Blue_Falcon", Vehicle::Blue_Falcon)
        .value("Sprinter", Vehicle::Sprinter)
        .value("Honeycoupe", Vehicle::Honeycoupe)
        .value("Standard_Bike_S", Vehicle::Standard_Bike_S)
        .value("Standard_Bike_M", Vehicle::Standard_Bike_M)
        .value("Standard_Bike_L", Vehicle::Standard_Bike_L)
        .value("Bullet_Bike", Vehicle::Bullet_Bike)
        .value("Mach_Bike", Vehicle::Mach_Bike)
        .value("Flame_Runner", Vehicle::Flame_Runner)
        .value("Bit_Bike", Vehicle::Bit_Bike)
        .value("Sugarscoot", Vehicle::Sugarscoot)
        .value("Wario_Bike", Vehicle::Wario_Bike)
        .value("Quacker", Vehicle::Quacker)
        .value("Zip_Zip", Vehicle::Zip_Zip)
        .value("Shooting_Star", Vehicle::Shooting_Star)
        .value("Magikruiser", Vehicle::Magikruiser)
        .value("Sneakster", Vehicle::Sneakster)
        .value("Spear", Vehicle::Spear)
        .value("Jet_Bubble", Vehicle::Jet_Bubble)
        .value("Dolphin_Dasher", Vehicle::Dolphin_Dasher)
        .value("Phantom", Vehicle::Phantom)
        .value("Max", Vehicle::Max);

    nb::enum_<Character>(m, "Character")
        .value("Mario", Character::Mario)
        .value("Baby_Peach", Character::Baby_Peach)
        .value("Waluigi", Character::Waluigi)
        .value("Bowser", Character::Bowser)
        .value("Baby_Daisy", Character::Baby_Daisy)
        .value("Dry_Bones", Character::Dry_Bones)
        .value("Baby_Mario", Character::Baby_Mario)
        .value("Luigi", Character::Luigi)
        .value("Toad", Character::Toad)
        .value("Donkey_Kong", Character::Donkey_Kong)
        .value("Yoshi", Character::Yoshi)
        .value("Wario", Character::Wario)
        .value("Baby_Luigi", Character::Baby_Luigi)
        .value("Toadette", Character::Toadette)
        .value("Koopa_Troopa", Character::Koopa_Troopa)
        .value("Daisy", Character::Daisy)
        .value("Peach", Character::Peach)
        .value("Birdo", Character::Birdo)
        .value("Diddy_Kong", Character::Diddy_Kong)
        .value("King_Boo", Character::King_Boo)
        .value("Bowser_Jr", Character::Bowser_Jr)
        .value("Dry_Bowser", Character::Dry_Bowser)
        .value("Funky_Kong", Character::Funky_Kong)
        .value("Rosalina", Character::Rosalina)
        .value("Small_Mii_Outfit_A_Male", Character::Small_Mii_Outfit_A_Male)
        .value("Small_Mii_Outfit_A_Female", Character::Small_Mii_Outfit_A_Female)
        .value("Small_Mii_Outfit_B_Male", Character::Small_Mii_Outfit_B_Male)
        .value("Small_Mii_Outfit_B_Female", Character::Small_Mii_Outfit_B_Female)
        .value("Small_Mii_Outfit_C_Male", Character::Small_Mii_Outfit_C_Male)
        .value("Small_Mii_Outfit_C_Female", Character::Small_Mii_Outfit_C_Female)
        .value("Medium_Mii_Outfit_A_Male", Character::Medium_Mii_Outfit_A_Male)
        .value("Medium_Mii_Outfit_A_Female", Character::Medium_Mii_Outfit_A_Female)
        .value("Medium_Mii_Outfit_B_Male", Character::Medium_Mii_Outfit_B_Male)
        .value("Medium_Mii_Outfit_B_Female", Character::Medium_Mii_Outfit_B_Female)
        .value("Medium_Mii_Outfit_C_Male", Character::Medium_Mii_Outfit_C_Male)
        .value("Medium_Mii_Outfit_C_Female", Character::Medium_Mii_Outfit_C_Female)
        .value("Large_Mii_Outfit_A_Male", Character::Large_Mii_Outfit_A_Male)
        .value("Large_Mii_Outfit_A_Female", Character::Large_Mii_Outfit_A_Female)
        .value("Large_Mii_Outfit_B_Male", Character::Large_Mii_Outfit_B_Male)
        .value("Large_Mii_Outfit_B_Female", Character::Large_Mii_Outfit_B_Female)
        .value("Large_Mii_Outfit_C_Male", Character::Large_Mii_Outfit_C_Male)
        .value("Large_Mii_Outfit_C_Female", Character::Large_Mii_Outfit_C_Female)
        .value("Medium_Mii", Character::Medium_Mii)
        .value("Small_Mii", Character::Small_Mii)
        .value("Large_Mii", Character::Large_Mii)
        .value("Peach_Biker_Outfit", Character::Peach_Biker_Outfit)
        .value("Daisy_Biker_Outfit", Character::Daisy_Biker_Outfit)
        .value("Rosalina_Biker_Outfit", Character::Rosalina_Biker_Outfit)
        .value("Max", Character::Max);

    using VecNumpy = nb::ndarray<float, nb::numpy, nb::shape<3>, nb::f_contig>;
    nb::class_<EGG::Vector3f>(m, "Vector3f")
        .def(nb::init<>())  // Default constructor
        .def(nb::init<float, float, float>(), nb::arg("x"), nb::arg("y"), nb::arg("z"))
        .def_rw("x", &EGG::Vector3f::x)
        .def_rw("y", &EGG::Vector3f::y)
        .def_rw("z", &EGG::Vector3f::z)
        .def("__repr__", [](const EGG::Vector3f &v) {
            return "<Vector3f(x=" + std::to_string(v.x) +
                   ", y=" + std::to_string(v.y) +
                   ", z=" + std::to_string(v.z) + ")>";
        })
        .def("to_numpy", [](const EGG::Vector3f &v) {
            // Convert to a NumPy array
            return VecNumpy(reinterpret_cast<void*>(const_cast<f32*>(&v.x)));
        }, nb::rv_policy::reference_internal)
        .def_static("from_numpy", [](VecNumpy array) {
            // Convert from a NumPy array
            if (array.size() != 3) {
                throw std::runtime_error("NumPy array must have 3 elements");
            }
            return EGG::Vector3f(array(0), array(1), array(2));
        });

    using QuatNumpy = nb::ndarray<float, nb::numpy, nb::shape<4>, nb::f_contig>;
    nb::class_<EGG::Quatf>(m, "Quatf")
        .def(nb::init<>())
        .def(nb::init<float, float, float, float>(), nb::arg("x"), nb::arg("y"), nb::arg("z"), nb::arg("w"))
        .def_prop_rw("x", [](EGG::Quatf &q) { return q.v.x; }, [](EGG::Quatf &q, float value) { q.v.x = value; })
        .def_prop_rw("y", [](EGG::Quatf &q) { return q.v.y; }, [](EGG::Quatf &q, float value) { q.v.y = value; })
        .def_prop_rw("z", [](EGG::Quatf &q) { return q.v.z; }, [](EGG::Quatf &q, float value) { q.v.z = value; })
        .def_rw("w", &EGG::Quatf::w)
        .def("__repr__", [](const EGG::Quatf &q) {
            return "<Quatf(x=" + std::to_string(q.v.x) +
                   ", y=" + std::to_string(q.v.y) +
                   ", z=" + std::to_string(q.v.z) +
                   ", w=" + std::to_string(q.w) + ")>";
        })
        .def("to_numpy", [](const EGG::Quatf &q) {
            // Convert to a NumPy array (x, y, z, w)
            return QuatNumpy(reinterpret_cast<void*>(const_cast<f32*>(&q.v.x)));
        }, nb::rv_policy::reference)
        .def_static("from_numpy", [](QuatNumpy array) {
            // Convert from a NumPy array (x, y, z, w)
            if (array.size() != 4) {
                throw std::runtime_error("NumPy array must have 4 elements");
            }
            return EGG::Quatf(array(3), array(0), array(1), array(2));
        });

    nb::class_<Kart::KartMove> kartMove(m, "KartMove");
    kartMove
        .def("kclSpeedFactor", &Kart::KartMove::kclSpeedFactor)
        .def("kclRotFactor", &Kart::KartMove::kclRotFactor)
        .def("driftState", &Kart::KartMove::driftState)
        .def("mtCharge", &Kart::KartMove::mtCharge);

    nb::enum_<Kart::KartMove::DriftState>(kartMove, "DriftState")
        .value("NotDrifting", Kart::KartMove::DriftState::NotDrifting)
        .value("ChargingMt", Kart::KartMove::DriftState::ChargingMt)
        .value("ChargedMt", Kart::KartMove::DriftState::ChargedMt)
        .value("ChargedSmt", Kart::KartMove::DriftState::ChargedSmt);

    nb::class_<Timer>(m, "Timer")
        .def(nb::init<>())
        .def(nb::init<u16, u8, u16>())
        .def(nb::init<u32>())
        .def("__eq__", &Timer::operator==)
        .def("__ne__", &Timer::operator!=)
        .def("__lt__", [](const Timer &self, const Timer &other) { return (self <=> other) < 0; })
        .def("__le__", [](const Timer &self, const Timer &other) { return (self <=> other) <= 0; })
        .def("__gt__", [](const Timer &self, const Timer &other) { return (self <=> other) > 0; })
        .def("__ge__", [](const Timer &self, const Timer &other) { return (self <=> other) >= 0; })
        .def("__sub__", &Timer::operator-)
        .def("__add__", &Timer::operator+)
        .def_rw("min", &Timer::min)
        .def_rw("sec", &Timer::sec)
        .def_rw("mil", &Timer::mil)
        .def_rw("valid", &Timer::valid);

    nb::class_<RaceManager::Player>(m, "Player")
        .def("getLapSplit", &RaceManager::Player::getLapSplit)
        .def("checkpointId", &RaceManager::Player::checkpointId)
        .def("raceCompletion", &RaceManager::Player::raceCompletion)
        .def("jugemId", &RaceManager::Player::jugemId)
        .def("lapTimers", &RaceManager::Player::lapTimers, nb::rv_policy::reference)
        .def("lapTimer", &RaceManager::Player::lapTimer, nb::rv_policy::reference)
        .def("raceTimer", &RaceManager::Player::raceTimer, nb::rv_policy::reference);

    nb::class_<System::RaceManager> raceManager(m, "RaceManager");
    raceManager
        .def("player", &System::RaceManager::player, nb::rv_policy::reference)
        .def("stage", &System::RaceManager::stage);

    nb::enum_<System::RaceManager::Stage>(raceManager, "Stage")
        .value("Intro", System::RaceManager::Stage::Intro)
        .value("Countdown", System::RaceManager::Stage::Countdown)
        .value("Race", System::RaceManager::Stage::Race)
        .value("FinishLocal", System::RaceManager::Stage::FinishLocal)
        .value("FinishGlobal", System::RaceManager::Stage::FinishGlobal);

    nb::class_<Item::ItemDirector>(m, "ItemDirector")
        .def("itemInventory", &Item::ItemDirector::itemInventory);

    nb::class_<Item::ItemInventory>(m, "ItemInventory")
        .def("id", &Item::ItemInventory::id)
        .def("currentCount", &Item::ItemInventory::currentCount);

    nb::enum_<Item::ItemId>(m, "ItemId")
        .value("TRIPLE_MUSHROOM", Item::ItemId::TRIPLE_MUSHROOM)
        .value("NONE", Item::ItemId::NONE);

    nb::class_<Kart::KartState>(m, "KartState")
        .def("isDrifting", &Kart::KartState::isDrifting)
        .def("isVehicleBodyFloorCollision", &Kart::KartState::isVehicleBodyFloorCollision)
        .def("isAnyWheelCollision", &Kart::KartState::isAnyWheelCollision)
        .def("isAllWheelsCollision", &Kart::KartState::isAllWheelsCollision)
        .def("isStickyRoad", &Kart::KartState::isStickyRoad)
        .def("isTouchingGround", &Kart::KartState::isTouchingGround)
        .def("isChargeStartBoost", &Kart::KartState::isChargeStartBoost)
        .def("isBoost", &Kart::KartState::isBoost)
        .def("isMushroomBoost", &Kart::KartState::isMushroomBoost)
        .def("isWheelie", &Kart::KartState::isWheelie)
        .def("isRampBoost", &Kart::KartState::isRampBoost)
        .def("isJumpPad", &Kart::KartState::isJumpPad)
        .def("isInCannon", &Kart::KartState::isInCannon)
        .def("isInATrick", &Kart::KartState::isInATrick)
        .def("isBoostOffroadInvincibility", &Kart::KartState::isBoostOffroadInvincibility)
        .def("isHalfPipeRamp", &Kart::KartState::isHalfPipeRamp)
        .def("isOverZipper", &Kart::KartState::isOverZipper)
        .def("isZipperBoost", &Kart::KartState::isZipperBoost)
        .def("isZipperTrick", &Kart::KartState::isZipperTrick)
        .def("isBurnout", &Kart::KartState::isBurnout)
        .def("isTrickable", &Kart::KartState::isTrickable)
        .def("isWheelieRot", &Kart::KartState::isWheelieRot)
        .def("startBoostCharge", &Kart::KartState::startBoostCharge);

    nb::class_<Kart::KartObjectProxy>(m, "KartObjectProxy")
        .def("state", static_cast<const Kart::KartState *(Kart::KartObjectProxy::*)() const>(&Kart::KartObjectProxy::state), nb::rv_policy::reference)
        .def("move", static_cast<const Kart::KartMove *(Kart::KartObjectProxy::*)() const>(&Kart::KartObjectProxy::move), nb::rv_policy::reference)
        .def("pos", &Kart::KartObjectProxy::pos, nb::rv_policy::reference)
        .def("prev_pos", &Kart::KartObjectProxy::prevPos, nb::rv_policy::reference)
        .def("full_rot", &Kart::KartObjectProxy::fullRot, nb::rv_policy::reference)
        .def("ext_vel", &Kart::KartObjectProxy::extVel, nb::rv_policy::reference)
        .def("int_vel", &Kart::KartObjectProxy::intVel, nb::rv_policy::reference)
        .def("velocity", &Kart::KartObjectProxy::velocity, nb::rv_policy::reference)
        .def("speed", &Kart::KartObjectProxy::speed)
        .def("acceleration", &Kart::KartObjectProxy::acceleration)
        .def("soft_speed_limit", &Kart::KartObjectProxy::softSpeedLimit)
        .def("main_rot", &Kart::KartObjectProxy::mainRot, nb::rv_policy::reference)
        .def("ang_vel_2", &Kart::KartObjectProxy::angVel2, nb::rv_policy::reference)
        .def("is_bike", &Kart::KartObjectProxy::isBike)
        .def("speed_ratio", &Kart::KartObjectProxy::speedRatio)
        .def("speed_ratio_capped", &Kart::KartObjectProxy::speedRatioCapped)
        .def("is_in_respawn", &Kart::KartObjectProxy::isInRespawn)
        .def("__repr__", [](const Kart::KartObjectProxy &kop) {
            auto vec3f_to_string = [](const EGG::Vector3f &v) {
                return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
            };
            auto quatf_to_string = [](const EGG::Quatf &q) {
                return "(" + std::to_string(q.v.x) + ", " + std::to_string(q.v.y) + ", " + std::to_string(q.v.z) + ", " + std::to_string(q.w) + ")";
            };

            std::ostringstream oss;
            oss << "<KartObjectProxy("
                << "pos=" << vec3f_to_string(kop.pos()) << ", "
                << "main_rot=" << quatf_to_string(kop.mainRot()) << ", "
                << "int_vel=" << vec3f_to_string(kop.intVel()) << ", "
                << "ext_vel=" << vec3f_to_string(kop.extVel()) << ", "
                << "ang_vel_2=" << vec3f_to_string(kop.angVel2())
                << ")>";
            return oss.str();
        });

    m.attr("KPAD_BUTTON_A") = ButtonInput::KPAD_BUTTON_A;
    m.attr("KPAD_BUTTON_B") = ButtonInput::KPAD_BUTTON_B;
    m.attr("KPAD_BUTTON_ITEM") = ButtonInput::KPAD_BUTTON_ITEM;

    m.def("buttonInput", encode_buttons, nb::arg("buttons"),
          "Encode an iterable of button values into a 2-byte signed short.");

    nb::class_<KHostSystem>(m, "KHostSystem")
        .def(nb::init<>())
        .def("configure", &KHostSystem::configure)
        .def("init", &KHostSystem::init)
        .def("setInput", &KHostSystem::setInput)
        .def("calc", &KHostSystem::calc)
        .def("reset", &KHostSystem::reset)
        .def("kartObjectProxy", &KHostSystem::kartObjectProxy, nb::rv_policy::reference)
        .def("raceManager", &KHostSystem::raceManager, nb::rv_policy::reference)
        .def("itemDirector", &KHostSystem::itemDirector, nb::rv_policy::reference)
        .def("raceCompletion", &KHostSystem::raceCompletion);
}
