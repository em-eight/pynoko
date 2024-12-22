#include <Common.hh>

#include <game/system/KPadDirector.hh>
#include <game/system/RaceConfig.hh>

#include <egg/math/Vector.hh>
#include <egg/math/Quat.hh>
#include <egg/core/ExpHeap.hh>
#include <egg/core/SceneManager.hh>

#include <game/kart/KartObjectManager.hh>
#include <game/kart/KartDynamics.hh>
#include <game/system/RaceManager.hh>

#include <host/SceneCreatorDynamic.hh>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

namespace py = pybind11;

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
// _ctors/_dtors
HeapManager heapMgr;

class KHostSystem {
  KPadHostController controller;
  EGG::SceneManager *m_sceneMgr;

public:
  void configure(Course course, Character character, Vehicle vehicle, bool isAuto);
  void init();
  bool setInput(u16 buttons, u8 stickXRaw, u8 stickYRaw, Trick trick);
  void calc();
  void reset();
  const Kart::KartObjectProxy& kartObjectProxy();
  f32 raceCompletion();
};

void KHostSystem::configure(Course course, Character character, Vehicle vehicle, bool isAuto) {
    System::RaceConfig::RegisterInitCallback([course, character, vehicle, isAuto](System::RaceConfig *config, void *arg) {
        config->raceScenario().players[0].type = System::RaceConfig::Player::Type::Local;
        config->configure(course, character, vehicle, isAuto);
    }, nullptr);
}

void KHostSystem::init() {
    FlushDenormalsToZero();

    auto *sceneCreator = new SceneCreatorDynamic;
    m_sceneMgr = new EGG::SceneManager(sceneCreator);

    //System::RaceConfig::RegisterInitCallback(std::bind(&KHostSystem::OnInit, this, std::placeholders::_1, std::placeholders::_2), nullptr);

    m_sceneMgr->changeScene(0);
}

bool KHostSystem::setInput(u16 buttons, u8 stickXRaw, u8 stickYRaw, Trick trick) {
    return KPadDirector::Instance()->hostController()->setInputsRawStick(buttons, stickXRaw, stickYRaw, trick);
}

void KHostSystem::calc() {
    m_sceneMgr->calc();
}

void KHostSystem::reset() {
    m_sceneMgr->destroyScene(m_sceneMgr->currentScene());
    m_sceneMgr->createScene(2, m_sceneMgr->currentScene());
}

const Kart::KartObjectProxy& KHostSystem::kartObjectProxy() {
    return *Kart::KartObjectManager::Instance()->object(0);
}

f32 KHostSystem::raceCompletion() {
    const auto &player = System::RaceManager::Instance()->player();
    return player.raceCompletion();
}

class ButtonInput {
public:
    static constexpr int ACCELERATE = 0x1;
    static constexpr int BRAKE = 0x2;
    static constexpr int ITEM = 0x4;
    static constexpr int DRIFT = 0x8;

    static short encode_buttons(const std::vector<int>& buttons) {
        short encoded = 0;
        for (int button : buttons) {
            encoded |= button;  // Combine the button values
        }
        return encoded;
    }
};

PYBIND11_MODULE(pynoko, m) {
    py::enum_<Trick>(m, "Trick")
        .value("NoTrick", Trick::None)
        .value("Up", Trick::Up)
        .value("Down", Trick::Down)
        .value("Left", Trick::Left)
        .value("Right", Trick::Right);

    py::enum_<Course>(m, "Course")
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

    py::enum_<Vehicle>(m, "Vehicle")
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

    py::enum_<Character>(m, "Character")
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

    py::class_<EGG::Vector3f>(m, "Vector3f")
        .def(py::init<>())  // Default constructor
        .def(py::init<float, float, float>(), py::arg("x"), py::arg("y"), py::arg("z"))
        .def_readwrite("x", &EGG::Vector3f::x)
        .def_readwrite("y", &EGG::Vector3f::y)
        .def_readwrite("z", &EGG::Vector3f::z)
        .def("__repr__", [](const EGG::Vector3f &v) {
            return "<Vector3f(x=" + std::to_string(v.x) +
                   ", y=" + std::to_string(v.y) +
                   ", z=" + std::to_string(v.z) + ")>";
        })
        .def("to_numpy", [](const EGG::Vector3f &v) {
            // Convert to a NumPy array
            return py::array_t<float>({3}, {sizeof(float)}, &v.x);
        })
        .def_static("from_numpy", [](py::array_t<float> array) {
            // Convert from a NumPy array
            if (array.size() != 3) {
                throw std::runtime_error("NumPy array must have 3 elements");
            }
            auto buf = array.unchecked<1>();
            return EGG::Vector3f(buf(0), buf(1), buf(2));
        });

    py::class_<EGG::Quatf>(m, "Quatf")
        .def(py::init<>())
        .def(py::init<float, float, float, float>(), py::arg("x"), py::arg("y"), py::arg("z"), py::arg("w"))
        .def_property("x", [](EGG::Quatf &q) { return q.v.x; }, [](EGG::Quatf &q, float value) { q.v.x = value; })
        .def_property("y", [](EGG::Quatf &q) { return q.v.y; }, [](EGG::Quatf &q, float value) { q.v.y = value; })
        .def_property("z", [](EGG::Quatf &q) { return q.v.z; }, [](EGG::Quatf &q, float value) { q.v.z = value; })
        .def_readwrite("w", &EGG::Quatf::w)
        .def("__repr__", [](const EGG::Quatf &q) {
            return "<Quatf(x=" + std::to_string(q.v.x) +
                   ", y=" + std::to_string(q.v.y) +
                   ", z=" + std::to_string(q.v.z) +
                   ", w=" + std::to_string(q.w) + ")>";
        })
        .def("to_numpy", [](const EGG::Quatf &q) {
            // Convert to a NumPy array (x, y, z, w)
            return py::array_t<float>({3}, {sizeof(float)}, &q.v.x);
        })
        .def_static("from_numpy", [](py::array_t<float> array) {
            // Convert from a NumPy array (x, y, z, w)
            if (array.size() != 4) {
                throw std::runtime_error("NumPy array must have 4 elements");
            }
            auto buf = array.unchecked<1>();
            return EGG::Quatf(buf(3), buf(0), buf(1), buf(2));
        });

    // I tried to make bindings for KartDynamics but turns out you can't bind polymorphic classes with RTTI off
    // due to missing typeinfo symbols...
    // So instead this stands as a reminder who has the same idea in the future
    /*py::class_<Kart::KartDynamics>(m, "KartDynamics")
        .def(py::init<>())  // Bind the constructor
        .def("inv_inertia_tensor", &Kart::KartDynamics::invInertiaTensor, py::return_value_policy::reference_internal)
        .def("ang_vel_0_factor", &Kart::KartDynamics::angVel0Factor)
        .def("pos", &Kart::KartDynamics::pos, py::return_value_policy::reference_internal)
        .def("velocity", &Kart::KartDynamics::velocity, py::return_value_policy::reference_internal)
        .def("gravity", &Kart::KartDynamics::gravity)
        .def("int_vel", &Kart::KartDynamics::intVel, py::return_value_policy::reference_internal)
        .def("main_rot", &Kart::KartDynamics::mainRot, py::return_value_policy::reference_internal)
        .def("full_rot", &Kart::KartDynamics::fullRot, py::return_value_policy::reference_internal)
        .def("total_force", &Kart::KartDynamics::totalForce, py::return_value_policy::reference_internal)
        .def("ext_vel", &Kart::KartDynamics::extVel, py::return_value_policy::reference_internal)
        .def("ang_vel_0", &Kart::KartDynamics::angVel0, py::return_value_policy::reference_internal)
        .def("ang_vel_2", &Kart::KartDynamics::angVel2, py::return_value_policy::reference_internal)
        .def("speed_fix", &Kart::KartDynamics::speedFix)
        .def("__repr__", [](const Kart::KartDynamics &kd) {
            auto vec3f_to_string = [](const EGG::Vector3f &v) {
                return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
            };
            auto quatf_to_string = [](const EGG::Quatf &q) {
                return "(" + std::to_string(q.v.x) + ", " + std::to_string(q.v.y) + ", " + std::to_string(q.v.z) + ", " + std::to_string(q.w) + ")";
            };

            std::ostringstream oss;
            oss << "<KartDynamics("
                << "pos=" << vec3f_to_string(kd.pos()) << ", "
                << "int_vel=" << vec3f_to_string(kd.intVel()) << ", "
                << "ext_vel=" << vec3f_to_string(kd.extVel()) << ", "
                << "main_rot=" << quatf_to_string(kd.mainRot()) << ", "  // Assuming Quatf has a Vector3f `v` for rotation
                << "ang_vel0=" << vec3f_to_string(kd.angVel0()) << ", "
                << "ang_vel2=" << vec3f_to_string(kd.angVel2())
                << ")>";
            return oss.str();
        });*/

    py::class_<Kart::KartObjectProxy>(m, "KartObjectProxy")
        .def("pos", &Kart::KartObjectProxy::pos, py::return_value_policy::reference_internal)
        .def("prev_pos", &Kart::KartObjectProxy::prevPos, py::return_value_policy::reference_internal)
        .def("full_rot", &Kart::KartObjectProxy::fullRot, py::return_value_policy::reference_internal)
        .def("ext_vel", &Kart::KartObjectProxy::extVel, py::return_value_policy::reference_internal)
        .def("int_vel", &Kart::KartObjectProxy::intVel, py::return_value_policy::reference_internal)
        .def("velocity", &Kart::KartObjectProxy::velocity, py::return_value_policy::reference_internal)
        .def("speed", &Kart::KartObjectProxy::speed)
        .def("acceleration", &Kart::KartObjectProxy::acceleration)
        .def("soft_speed_limit", &Kart::KartObjectProxy::softSpeedLimit)
        .def("main_rot", &Kart::KartObjectProxy::mainRot, py::return_value_policy::reference_internal)
        .def("ang_vel_2", &Kart::KartObjectProxy::angVel2, py::return_value_policy::reference_internal)
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

    m.attr("ACCELERATE") = ButtonInput::ACCELERATE;
    m.attr("BRAKE") = ButtonInput::BRAKE;
    m.attr("ITEM") = ButtonInput::ITEM;
    m.attr("DRIFT") = ButtonInput::DRIFT;

    m.def("buttonInput", &ButtonInput::encode_buttons, py::arg("buttons"),
          "Encode an iterable of button values into a 2-byte signed short.");

    py::class_<KHostSystem>(m, "KHostSystem")
        .def(py::init<>())
        .def("configure", &KHostSystem::configure)
        .def("init", &KHostSystem::init)
        .def("setInput", &KHostSystem::setInput)
        .def("calc", &KHostSystem::calc)
        .def("reset", &KHostSystem::reset)
        .def("kartObjectProxy", &KHostSystem::kartObjectProxy, py::return_value_policy::reference_internal)
        .def("raceCompletion", &KHostSystem::raceCompletion);
}
