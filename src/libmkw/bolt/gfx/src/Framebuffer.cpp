#include "gfx/Framebuffer.hpp"
#include "gfx/gl_defines.h"

#include "util/common.h"

namespace bolt {
namespace gfx {

Framebuffer::Framebuffer(int width, int height) : mWidth(width), mHeight(height) {
    glGenFramebuffers(1, &fbo);

    // color render buffer
    glGenRenderbuffers(1, &rboColor);
    glBindRenderbuffer(GL_RENDERBUFFER, rboColor);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB32F, mWidth, mHeight);

    // depth render buffer
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mWidth, mHeight);

    // attach render buffers
    GLint previousFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rboColor);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    RUNTIME_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Incomplete framebuffer");
    glBindFramebuffer(GL_FRAMEBUFFER, previousFBO);
}

Framebuffer::~Framebuffer() {
    // deattach render buffers
    GLint previousFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, previousFBO);

    glDeleteRenderbuffers(1, &rboColor);
    glDeleteRenderbuffers(1, &rboDepth);

    glDeleteFramebuffers(1, &fbo);
}

void Framebuffer::use() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
}

void Framebuffer::useEmbedded() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Framebuffer::readBuffer(void* data) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glReadPixels(0, 0, mWidth, mHeight, GL_RGB, GL_FLOAT, data);
}

} // gfx
} // bolt
