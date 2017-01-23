#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gl/Gl.h>
#include <gl/Glu.h>
#include "Thirdparty/OpenGL/glext.h"
#include "ThirdParty/OpenGL/wglext.h"

extern PFNGLGENBUFFERSPROC              glGenBuffers;
extern PFNGLDELETEBUFFERSPROC           glDeleteBuffers;
extern PFNGLBINDBUFFERPROC              glBindBuffer;
extern PFNGLBUFFERDATAPROC              glBufferData;
extern PFNGLCREATESHADERPROC            glCreateShader;
extern PFNGLSHADERSOURCEPROC            glShaderSource;
extern PFNGLCOMPILESHADERPROC           glCompileShader;
extern PFNGLGETSHADERIVPROC             glGetShaderiv;
extern PFNGLDELETESHADERPROC            glDeleteShader;
extern PFNGLCREATEPROGRAMPROC           glCreateProgram;
extern PFNGLATTACHSHADERPROC            glAttachShader;
extern PFNGLLINKPROGRAMPROC             glLinkProgram;
extern PFNGLGETPROGRAMIVPROC            glGetProgramiv;
extern PFNGLDETACHSHADERPROC            glDetachShader;
extern PFNGLDELETEPROGRAMPROC           glDeleteProgram;
extern PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog;
extern PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog;
extern PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
extern PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
extern PFNGLGETATTRIBLOCATIONPROC       glGetAttribLocation;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
extern PFNGLVERTEXATTRIBIPOINTERPROC     glVertexAttribIPointer;
extern PFNGLUSEPROGRAMPROC              glUseProgram;

extern PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation;
extern PFNGLUNIFORM1FVPROC              glUniform1fv;
extern PFNGLUNIFORM2FVPROC              glUniform2fv;
extern PFNGLUNIFORM3FVPROC              glUniform3fv;
extern PFNGLUNIFORM4FVPROC              glUniform4fv;
extern PFNGLUNIFORM1IVPROC              glUniform1iv;
extern PFNGLUNIFORM2IVPROC              glUniform2iv;
extern PFNGLUNIFORM3IVPROC              glUniform3iv;
extern PFNGLUNIFORM4IVPROC              glUniform4iv;
extern PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv;

extern PFNGLGENSAMPLERSPROC             glGenSamplers;
extern PFNGLSAMPLERPARAMETERIPROC       glSamplerParameteri;

extern PFNGLACTIVETEXTUREPROC           glActiveTexture;
extern PFNGLBINDSAMPLERPROC             glBindSampler;
extern PFNGLUNIFORM1IPROC               glUniform1i;
extern PFNGLUNIFORM1FPROC               glUniform1f;

extern PFNGLGETACTIVEATTRIBPROC         glGetActiveAttrib;
extern PFNGLGETACTIVEUNIFORMPROC        glGetActiveUniform;

extern PFNGLGENFRAMEBUFFERSPROC         glGenFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC         glBindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTUREPROC      glFramebufferTexture;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC  glCheckFramebufferStatus;
extern PFNGLDELETEFRAMEBUFFERSPROC      glDeleteFramebuffers;
extern PFNGLDRAWBUFFERSPROC             glDrawBuffers;
extern PFNGLBLITFRAMEBUFFERPROC         glBlitFramebuffer;