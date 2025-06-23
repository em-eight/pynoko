#pragma once

namespace bolt {
namespace gfx {

/**
 * Represents an external framebuffer we can perform draw calls on, and then read the data as image
 */
class Framebuffer {
public:
    Framebuffer(int width, int height);
    ~Framebuffer();
    /// Binds this framebuffer, so that all subsequent draw calls write to it
    void use();
    /// Binds the embedded framebuffer. This is the framebuffer used by default, but in order to revert to it this function can be called
    static void useEmbedded();
    int bufferSize() const { return mWidth * mHeight * 3 * sizeof(float); }
    /// Read framebuffer pixels. Format is packed RGB
    void readBuffer(void* data);

private:
    int mWidth;
    int mHeight;
    unsigned int fbo;
    unsigned int rboColor;
    unsigned int rboDepth;
};

} // gfx
} // bolt
