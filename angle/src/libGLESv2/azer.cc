#include "precompiled.h"

#include "libGLESv2/main.h"
#include "libGLESv2/Context.h"
#include "libGLESv2/Framebuffer.h"
#include "libGLESv2/Renderbuffer.h"
#include "libGLESv2/renderer/RenderTarget.h"
#include "libGLESv2/renderer/d3d11/Renderer11.h"
#include "libGLESv2/renderer/d3d11/RenderTarget11.h"
#include "libGLESv2/renderer/d3d11/TextureStorage11.h"

HANDLE createSharedHandle(ID3D11Resource* resource) {
  HANDLE shared_handle;
  HRESULT hr;
  IDXGIResource* dxgi_res = NULL;
  hr = resource->QueryInterface(__uuidof(IDXGIResource), (void**)&dxgi_res);
  if (FAILED(hr)) {
    return NULL;
  }

  hr = dxgi_res->GetSharedHandle(&shared_handle);
  if (dxgi_res) {dxgi_res->Release();}
  if (FAILED(hr)) {
    return NULL;
  }

  return shared_handle;
}

void glGetTexShareD3DTex(GLenum target, GLuint fbhandle, void** val) {
  gl::Context *context = gl::getNonLostContext();
  if (target != GL_DRAW_FRAMEBUFFER_ANGLE) {
    return gl::error(GL_INVALID_ENUM);
  }

  gl::Texture* fb = context->getTexture(fbhandle);
  if (fb == NULL) {
    return gl::error(GL_INVALID_ENUM);
  }

  rx::TextureStorageInterface* storage_intf = fb->getNativeTexture();
  rx::TextureStorage11* storage = (rx::TextureStorage11*)
      storage_intf->getStorageInstance();
  ID3D11Texture2D* resource = storage->getBaseTexture();
  *val = createSharedHandle(resource);
}
