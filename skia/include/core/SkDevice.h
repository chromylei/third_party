
/*
 * Copyright 2010 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#ifndef SkDevice_DEFINED
#define SkDevice_DEFINED

#include "SkRefCnt.h"
#include "SkBitmap.h"
#include "SkCanvas.h"
#include "SkColor.h"
#include "SkDeviceProperties.h"
#include "SkImageFilter.h"

// getDeviceCapabilities() is not called by skia, but this flag keeps it around
// for clients that have "override" annotations on their subclass. These overrides
// should be deleted.
//#define SK_SUPPORT_LEGACY_GETDEVICECAPABILITIES

//#define SK_SUPPORT_LEGACY_COMPATIBLEDEVICE_CONFIG

class SkClipStack;
class SkDraw;
struct SkIRect;
class SkMatrix;
class SkMetaData;
class SkRegion;

class GrRenderTarget;

class SK_API SkBaseDevice : public SkRefCnt {
public:
    SK_DECLARE_INST_COUNT(SkBaseDevice)

    /**
     *  Construct a new device.
    */
    SkBaseDevice();

    /**
     *  Construct a new device.
    */
    SkBaseDevice(const SkDeviceProperties& deviceProperties);

    virtual ~SkBaseDevice();

#ifdef SK_SUPPORT_LEGACY_COMPATIBLEDEVICE_CONFIG
    /**
     *  Creates a device that is of the same type as this device (e.g. SW-raster,
     *  GPU, or PDF). The backing store for this device is created automatically
     *  (e.g. offscreen pixels or FBO or whatever is appropriate).
     *
     *  @param width    width of the device to create
     *  @param height   height of the device to create
     *  @param isOpaque performance hint, set to true if you know that you will
     *                  draw into this device such that all of the pixels will
     *                  be opaque.
     */
    SkBaseDevice* createCompatibleDevice(SkBitmap::Config config,
                                         int width, int height,
                                         bool isOpaque);
#endif
    SkBaseDevice* createCompatibleDevice(const SkImageInfo&);

    SkMetaData& getMetaData();

#ifdef SK_SUPPORT_LEGACY_GETDEVICECAPABILITIES
    enum Capabilities {
        kVector_Capability = 0x1,
    };
    virtual uint32_t getDeviceCapabilities() { return 0; }
#endif

    /** Return the width of the device (in pixels).
    */
    virtual int width() const = 0;
    /** Return the height of the device (in pixels).
    */
    virtual int height() const = 0;

    /** Return the image properties of the device. */
    virtual const SkDeviceProperties& getDeviceProperties() const {
        //Currently, all the properties are leaky.
        return fLeakyProperties;
    }

    /**
     *  Return ImageInfo for this device. If the canvas is not backed by pixels
     *  (cpu or gpu), then the info's ColorType will be kUnknown_SkColorType.
     */
    virtual SkImageInfo imageInfo() const;

    /**
     *  Return the bounds of the device in the coordinate space of the root
     *  canvas. The root device will have its top-left at 0,0, but other devices
     *  such as those associated with saveLayer may have a non-zero origin.
     */
    void getGlobalBounds(SkIRect* bounds) const {
        SkASSERT(bounds);
        const SkIPoint& origin = this->getOrigin();
        bounds->setXYWH(origin.x(), origin.y(), this->width(), this->height());
    }


    /** Returns true if the device's bitmap's config treats every pixel as
        implicitly opaque.
    */
    virtual bool isOpaque() const = 0;

    /** Return the bitmap config of the device's pixels
     */
    virtual SkBitmap::Config config() const = 0;

    /** Return the bitmap associated with this device. Call this each time you need
        to access the bitmap, as it notifies the subclass to perform any flushing
        etc. before you examine the pixels.
        @param changePixels set to true if the caller plans to change the pixels
        @return the device's bitmap
    */
    const SkBitmap& accessBitmap(bool changePixels);

#ifdef SK_SUPPORT_LEGACY_WRITEPIXELSCONFIG
    /**
     *  DEPRECATED: This will be made protected once WebKit stops using it.
     *              Instead use Canvas' writePixels method.
     *
     *  Similar to draw sprite, this method will copy the pixels in bitmap onto
     *  the device, with the top/left corner specified by (x, y). The pixel
     *  values in the device are completely replaced: there is no blending.
     *
     *  Currently if bitmap is backed by a texture this is a no-op. This may be
     *  relaxed in the future.
     *
     *  If the bitmap has config kARGB_8888_Config then the config8888 param
     *  will determines how the pixel valuess are intepreted. If the bitmap is
     *  not kARGB_8888_Config then this parameter is ignored.
     */
    virtual void writePixels(const SkBitmap& bitmap, int x, int y,
                             SkCanvas::Config8888 config8888 = SkCanvas::kNative_Premul_Config8888);
#endif

    bool writePixelsDirect(const SkImageInfo&, const void*, size_t rowBytes, int x, int y);

    void* accessPixels(SkImageInfo* info, size_t* rowBytes);

    /**
     * Return the device's associated gpu render target, or NULL.
     */
    virtual GrRenderTarget* accessRenderTarget() = 0;


    /**
     *  Return the device's origin: its offset in device coordinates from
     *  the default origin in its canvas' matrix/clip
     */
    const SkIPoint& getOrigin() const { return fOrigin; }

    /**
     * onAttachToCanvas is invoked whenever a device is installed in a canvas
     * (i.e., setDevice, saveLayer (for the new device created by the save),
     * and SkCanvas' SkBaseDevice & SkBitmap -taking ctors). It allows the
     * devices to prepare for drawing (e.g., locking their pixels, etc.)
     */
    virtual void onAttachToCanvas(SkCanvas*) {
        SkASSERT(!fAttachedToCanvas);
        this->lockPixels();
#ifdef SK_DEBUG
        fAttachedToCanvas = true;
#endif
    };

    /**
     * onDetachFromCanvas notifies a device that it will no longer be drawn to.
     * It gives the device a chance to clean up (e.g., unlock its pixels). It
     * is invoked from setDevice (for the displaced device), restore and
     * possibly from SkCanvas' dtor.
     */
    virtual void onDetachFromCanvas() {
        SkASSERT(fAttachedToCanvas);
        this->unlockPixels();
#ifdef SK_DEBUG
        fAttachedToCanvas = false;
#endif
    };

protected:
    enum Usage {
       kGeneral_Usage,
       kSaveLayer_Usage  // <! internal use only
    };

    struct TextFlags {
        uint32_t            fFlags;     // SkPaint::getFlags()
        SkPaint::Hinting    fHinting;
    };

    /**
     *  Device may filter the text flags for drawing text here. If it wants to
     *  make a change to the specified values, it should write them into the
     *  textflags parameter (output) and return true. If the paint is fine as
     *  is, then ignore the textflags parameter and return false.
     *
     *  The baseclass SkBaseDevice filters based on its depth and blitters.
     */
    virtual bool filterTextFlags(const SkPaint& paint, TextFlags*) = 0;

    /**
     *
     *  DEPRECATED: This will be removed in a future change. Device subclasses
     *  should use the matrix and clip from the SkDraw passed to draw functions.
     *
     *  Called with the correct matrix and clip before this device is drawn
     *  to using those settings. If your subclass overrides this, be sure to
     *  call through to the base class as well.
     *
     *  The clipstack is another view of the clip. It records the actual
     *  geometry that went into building the region. It is present for devices
     *  that want to parse it, but is not required: the region is a complete
     *  picture of the current clip. (i.e. if you regionize all of the geometry
     *  in the clipstack, you will arrive at an equivalent region to the one
     *  passed in).
     */
     virtual void setMatrixClip(const SkMatrix&, const SkRegion&,
                                const SkClipStack&) {};

    /** Clears the entire device to the specified color (including alpha).
     *  Ignores the clip.
     */
    virtual void clear(SkColor color) = 0;

    SK_ATTR_DEPRECATED("use clear() instead")
    void eraseColor(SkColor eraseColor) { this->clear(eraseColor); }

    /** These are called inside the per-device-layer loop for each draw call.
     When these are called, we have already applied any saveLayer operations,
     and are handling any looping from the paint, and any effects from the
     DrawFilter.
     */
    virtual void drawPaint(const SkDraw&, const SkPaint& paint) = 0;
    virtual void drawPoints(const SkDraw&, SkCanvas::PointMode mode, size_t count,
                            const SkPoint[], const SkPaint& paint) = 0;
    virtual void drawRect(const SkDraw&, const SkRect& r,
                          const SkPaint& paint) = 0;
    virtual void drawOval(const SkDraw&, const SkRect& oval,
                          const SkPaint& paint) = 0;
    virtual void drawRRect(const SkDraw&, const SkRRect& rr,
                           const SkPaint& paint) = 0;

    // Default impl calls drawPath()
    virtual void drawDRRect(const SkDraw&, const SkRRect& outer,
                            const SkRRect& inner, const SkPaint&);

    /**
     *  If pathIsMutable, then the implementation is allowed to cast path to a
     *  non-const pointer and modify it in place (as an optimization). Canvas
     *  may do this to implement helpers such as drawOval, by placing a temp
     *  path on the stack to hold the representation of the oval.
     *
     *  If prePathMatrix is not null, it should logically be applied before any
     *  stroking or other effects. If there are no effects on the paint that
     *  affect the geometry/rasterization, then the pre matrix can just be
     *  pre-concated with the current matrix.
     */
    virtual void drawPath(const SkDraw&, const SkPath& path,
                          const SkPaint& paint,
                          const SkMatrix* prePathMatrix = NULL,
                          bool pathIsMutable = false) = 0;
    virtual void drawBitmap(const SkDraw&, const SkBitmap& bitmap,
                            const SkMatrix& matrix, const SkPaint& paint) = 0;
    virtual void drawSprite(const SkDraw&, const SkBitmap& bitmap,
                            int x, int y, const SkPaint& paint) = 0;

    /**
     *  The default impl. will create a bitmap-shader from the bitmap,
     *  and call drawRect with it.
     */
    virtual void drawBitmapRect(const SkDraw&, const SkBitmap&,
                                const SkRect* srcOrNull, const SkRect& dst,
                                const SkPaint& paint,
                                SkCanvas::DrawBitmapRectFlags flags) = 0;

    /**
     *  Does not handle text decoration.
     *  Decorations (underline and stike-thru) will be handled by SkCanvas.
     */
    virtual void drawText(const SkDraw&, const void* text, size_t len,
                          SkScalar x, SkScalar y, const SkPaint& paint) = 0;
    virtual void drawPosText(const SkDraw&, const void* text, size_t len,
                             const SkScalar pos[], SkScalar constY,
                             int scalarsPerPos, const SkPaint& paint) = 0;
    virtual void drawTextOnPath(const SkDraw&, const void* text, size_t len,
                                const SkPath& path, const SkMatrix* matrix,
                                const SkPaint& paint) = 0;
    virtual void drawVertices(const SkDraw&, SkCanvas::VertexMode, int vertexCount,
                              const SkPoint verts[], const SkPoint texs[],
                              const SkColor colors[], SkXfermode* xmode,
                              const uint16_t indices[], int indexCount,
                              const SkPaint& paint) = 0;
    /** The SkDevice passed will be an SkDevice which was returned by a call to
        onCreateDevice on this device with kSaveLayer_Usage.
     */
    virtual void drawDevice(const SkDraw&, SkBaseDevice*, int x, int y,
                            const SkPaint&) = 0;

    /**
     *  On success (returns true), copy the device pixels into the bitmap.
     *  On failure, the bitmap parameter is left unchanged and false is
     *  returned.
     *
     *  The device's pixels are converted to the bitmap's config. The only
     *  supported config is kARGB_8888_Config, though this is likely to be
     *  relaxed in  the future. The meaning of config kARGB_8888_Config is
     *  modified by the enum param config8888. The default value interprets
     *  kARGB_8888_Config as SkPMColor
     *
     *  If the bitmap has pixels already allocated, the device pixels will be
     *  written there. If not, bitmap->allocPixels() will be called
     *  automatically. If the bitmap is backed by a texture readPixels will
     *  fail.
     *
     *  The actual pixels written is the intersection of the device's bounds,
     *  and the rectangle formed by the bitmap's width,height and the specified
     *  x,y. If bitmap pixels extend outside of that intersection, they will not
     *  be modified.
     *
     *  Other failure conditions:
     *    * If the device is not a raster device (e.g. PDF) then readPixels will
     *      fail.
     *    * If bitmap is texture-backed then readPixels will fail. (This may be
     *      relaxed in the future.)
     */
    bool readPixels(SkBitmap* bitmap,
                    int x, int y,
                    SkCanvas::Config8888 config8888);

    ///////////////////////////////////////////////////////////////////////////

    /** Update as needed the pixel value in the bitmap, so that the caller can
        access the pixels directly.
        @return The device contents as a bitmap
    */
    virtual const SkBitmap& onAccessBitmap() = 0;

    /** Called when this device is installed into a Canvas. Balanced by a call
        to unlockPixels() when the device is removed from a Canvas.
    */
    virtual void lockPixels() = 0;
    virtual void unlockPixels() = 0;

    /**
     *  Returns true if the device allows processing of this imagefilter. If
     *  false is returned, then the filter is ignored. This may happen for
     *  some subclasses that do not support pixel manipulations after drawing
     *  has occurred (e.g. printing). The default implementation returns true.
     */
    virtual bool allowImageFilter(const SkImageFilter*) = 0;

    /**
     *  Override and return true for filters that the device can handle
     *  intrinsically. Doing so means that SkCanvas will pass-through this
     *  filter to drawSprite and drawDevice (and potentially filterImage).
     *  Returning false means the SkCanvas will have apply the filter itself,
     *  and just pass the resulting image to the device.
     */
    virtual bool canHandleImageFilter(const SkImageFilter*) = 0;

    /**
     *  Related (but not required) to canHandleImageFilter, this method returns
     *  true if the device could apply the filter to the src bitmap and return
     *  the result (and updates offset as needed).
     *  If the device does not recognize or support this filter,
     *  it just returns false and leaves result and offset unchanged.
     */
    virtual bool filterImage(const SkImageFilter*, const SkBitmap&,
                             const SkImageFilter::Context& ctx,
                             SkBitmap* result, SkIPoint* offset) = 0;

    // This is equal kBGRA_Premul_Config8888 or kRGBA_Premul_Config8888 if
    // either is identical to kNative_Premul_Config8888. Otherwise, -1.
    static const SkCanvas::Config8888 kPMColorAlias;

protected:
    // default impl returns NULL
    virtual SkSurface* newSurface(const SkImageInfo&);

    // default impl returns NULL
    virtual const void* peekPixels(SkImageInfo*, size_t* rowBytes);

    /**
     * Implements readPixels API. The caller will ensure that:
     *  1. bitmap has pixel config kARGB_8888_Config.
     *  2. bitmap has pixels.
     *  3. The rectangle (x, y, x + bitmap->width(), y + bitmap->height()) is
     *     contained in the device bounds.
     */
    virtual bool onReadPixels(const SkBitmap& bitmap,
                              int x, int y,
                              SkCanvas::Config8888 config8888);

    /**
     *  The caller is responsible for "pre-clipping" the src. The impl can assume that the src
     *  image at the specified x,y offset will fit within the device's bounds.
     *
     *  This is explicitly asserted in writePixelsDirect(), the public way to call this.
     */
    virtual bool onWritePixels(const SkImageInfo&, const void*, size_t, int x, int y);

    /**
     *  Default impl returns NULL.
     */
    virtual void* onAccessPixels(SkImageInfo* info, size_t* rowBytes);

    /**
     *  Leaky properties are those which the device should be applying but it isn't.
     *  These properties will be applied by the draw, when and as it can.
     *  If the device does handle a property, that property should be set to the identity value
     *  for that property, effectively making it non-leaky.
     */
    SkDeviceProperties fLeakyProperties;

private:
    friend class SkCanvas;
    friend struct DeviceCM; //for setMatrixClip
    friend class SkDraw;
    friend class SkDrawIter;
    friend class SkDeviceFilteredPaint;
    friend class SkDeviceImageFilterProxy;
    friend class SkDeferredDevice;    // for newSurface

    friend class SkSurface_Raster;

    // used to change the backend's pixels (and possibly config/rowbytes)
    // but cannot change the width/height, so there should be no change to
    // any clip information.
    // TODO: move to SkBitmapDevice
    virtual void replaceBitmapBackendForRasterSurface(const SkBitmap&) = 0;

    // just called by SkCanvas when built as a layer
    void setOrigin(int x, int y) { fOrigin.set(x, y); }
    // just called by SkCanvas for saveLayer
    SkBaseDevice* createCompatibleDeviceForSaveLayer(const SkImageInfo&);

#ifdef SK_SUPPORT_LEGACY_COMPATIBLEDEVICE_CONFIG
    /**
     * Justs exists during the period where clients still "override" this
     *  signature. They are supported by our base-impl calling this old
     *  signature from the new one (using ImageInfo).
     */
    virtual SkBaseDevice* onCreateCompatibleDevice(SkBitmap::Config config,
                                                   int width, int height,
                                                   bool isOpaque, Usage) {
        return NULL;
    }
#endif
    virtual SkBaseDevice* onCreateDevice(const SkImageInfo&, Usage) {
        return NULL;
    }

    /** Causes any deferred drawing to the device to be completed.
     */
    virtual void flush() = 0;

    SkIPoint    fOrigin;
    SkMetaData* fMetaData;

#ifdef SK_DEBUG
    bool        fAttachedToCanvas;
#endif

    typedef SkRefCnt INHERITED;
};

#endif
