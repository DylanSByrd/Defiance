#include "Engine/Renderer/OpenGLExtensions.hpp"

PFNGLGENBUFFERSPROC              glGenBuffers               = nullptr;
PFNGLDELETEBUFFERSPROC           glDeleteBuffers            = nullptr;
PFNGLBINDBUFFERPROC              glBindBuffer               = nullptr;
PFNGLBUFFERDATAPROC              glBufferData               = nullptr;
PFNGLCREATESHADERPROC            glCreateShader             = nullptr;
PFNGLSHADERSOURCEPROC            glShaderSource             = nullptr;
PFNGLCOMPILESHADERPROC           glCompileShader            = nullptr;
PFNGLGETSHADERIVPROC             glGetShaderiv              = nullptr;
PFNGLDELETESHADERPROC            glDeleteShader             = nullptr;
PFNGLCREATEPROGRAMPROC           glCreateProgram            = nullptr;
PFNGLATTACHSHADERPROC            glAttachShader             = nullptr;
PFNGLLINKPROGRAMPROC             glLinkProgram              = nullptr;
PFNGLGETPROGRAMIVPROC            glGetProgramiv             = nullptr;
PFNGLDETACHSHADERPROC            glDetachShader             = nullptr;
PFNGLDELETEPROGRAMPROC           glDeleteProgram            = nullptr;
PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog         = nullptr;
PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog        = nullptr;
PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays          = nullptr;
PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays       = nullptr;
PFNGLBINDVERTEXARRAYPROC         glBindVertexArray          = nullptr;
PFNGLGETATTRIBLOCATIONPROC       glGetAttribLocation        = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray  = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer      = nullptr;
PFNGLVERTEXATTRIBIPOINTERPROC    glVertexAttribIPointer     = nullptr;
PFNGLUSEPROGRAMPROC              glUseProgram               = nullptr;
                                                              
PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation       = nullptr;
PFNGLUNIFORM1FVPROC              glUniform1fv               = nullptr;
PFNGLUNIFORM2FVPROC              glUniform2fv               = nullptr;
PFNGLUNIFORM3FVPROC              glUniform3fv               = nullptr;
PFNGLUNIFORM4FVPROC              glUniform4fv               = nullptr;
PFNGLUNIFORM1IVPROC              glUniform1iv               = nullptr;
PFNGLUNIFORM2IVPROC              glUniform2iv               = nullptr;
PFNGLUNIFORM3IVPROC              glUniform3iv               = nullptr;
PFNGLUNIFORM4IVPROC              glUniform4iv               = nullptr;
PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv         = nullptr;

PFNGLGENSAMPLERSPROC             glGenSamplers              = nullptr;
PFNGLSAMPLERPARAMETERIPROC       glSamplerParameteri        = nullptr;

PFNGLACTIVETEXTUREPROC           glActiveTexture            = nullptr;
PFNGLBINDSAMPLERPROC             glBindSampler              = nullptr;
PFNGLUNIFORM1IPROC               glUniform1i                = nullptr;
PFNGLUNIFORM1FPROC               glUniform1f                = nullptr;

PFNGLGETACTIVEATTRIBPROC         glGetActiveAttrib          = nullptr;
PFNGLGETACTIVEUNIFORMPROC        glGetActiveUniform         = nullptr;

PFNGLGENFRAMEBUFFERSPROC         glGenFramebuffers          = nullptr;
PFNGLBINDFRAMEBUFFERPROC         glBindFramebuffer          = nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC      glFramebufferTexture       = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC  glCheckFramebufferStatus   = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC      glDeleteFramebuffers       = nullptr;
PFNGLDRAWBUFFERSPROC             glDrawBuffers              = nullptr;
PFNGLBLITFRAMEBUFFERPROC         glBlitFramebuffer          = nullptr;