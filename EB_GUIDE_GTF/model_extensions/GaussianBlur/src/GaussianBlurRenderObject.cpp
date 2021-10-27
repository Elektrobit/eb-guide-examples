////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
//
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
///////////////////////////////////////////////////////////////////////////////

#include "GaussianBlurRenderObject.h"

using gtf::scdr::Allocator;
using gtf::scdr::FramebufferDescriptor;
using gtf::scdr::RenderObjectStackFrame;
using gtf::scdr::TouchObject;

namespace {
bool operator==(const GaussianBlurWeightParams& lhs, const GaussianBlurWeightParams& rhs)
{
    return lhs.width == rhs.width
           && lhs.height == rhs.height
           && lhs.sigma == rhs.sigma;
}

bool operator!=(const GaussianBlurWeightParams& lhs, const GaussianBlurWeightParams& rhs)
{
    return !(lhs == rhs);
}

GaussianBlurWeightParams makeDefaultParams()
{
    GaussianBlurWeightParams params;
    memset(&params, 0, sizeof(params));
    return params;
}

GaussianBlurWeightParams makeParams(uint32_t width_, uint32_t height_, float sigma_)
{
    GaussianBlurWeightParams params;
    memset(&params, 0, sizeof(params));
    params.width = width_;
    params.height = height_;
    params.sigma = sigma_;
    return params;
}

} // namespace

GaussianBlurRenderObject::GaussianBlurRenderObject(gtf::properties::Container& model_, gtf::scdr::GraphicsApi api_, const gtf::scdr::WidgetFeaturesCommon2D& features_, const gtf::scdr::WidgetPropertiesCommon2D& properties_)
    : RenderObjectBase(model_, features_, properties_)
    , next(NULL)
    , api(api_)
    , framebuffer(NULL)
    , framebufferId(0)
    , offscreenFramebufferId(0)
    , drawId(0)
    , alphaMask(NULL)
    , fbParent(nullptr)
    , fbChildren(nullptr)
{
    memset(&effectUniforms, 0, sizeof(effectUniforms));
    memset(&currentWeightParams, 0, sizeof(currentWeightParams));

    memset(fbBlur, 0, sizeof(fbBlur));

    setDefaultWeights();
}

auto GaussianBlurRenderObject::createStackFrame(RenderObjectStackFrame& parent_, Allocator& allocator_) -> RenderObjectStackFrame*
{
    GTF_UNUSED_PARAM(parent_);

    return ALLOC_OBJECT(allocator_, GaussianBlurStackFrame);
}

auto GaussianBlurRenderObject::destroyStackFrame(RenderObjectStackFrame* frame_, Allocator& allocator_) -> void
{
    FREE_OBJECT(allocator_, static_cast<GaussianBlurStackFrame*>(frame_));
}

auto GaussianBlurRenderObject::getTouchObject() -> const TouchObject*
{
    if (featureValues.mask & gtf::scdr::WidgetFeaturesCommon2DValues::Touch)
    {
        return this;
    }
    return NULL;
}

auto GaussianBlurRenderObject::getBoundingRectangle(gtf::scdr::Rectangle<int16_t>& rectOut_) const -> void
{
    gtf::scdr::CalculateBoundingRectangle(*this, rectOut_);
}

auto GaussianBlurRenderObject::intersect(const gtf::scdr::ScreenPosition& pos_) const -> bool
{
    return gtf::scdr::IntersectsBoundingRectangle(*this, pos_);
}

auto GaussianBlurRenderObject::updateFramebuffer(
    const gtf::scdr::Framebuffer* fbParent_,
    int32_t                       width_,
    int32_t                       height_,
    gtf::scdr::DataStore&         dataStore_,
    gtf::scdr::CommandBuffer&     commandBuffer_) -> bool
{
    const uint32_t fbWidth  = static_cast<uint32_t>(width_);
    const uint32_t fbHeight = static_cast<uint32_t>(height_);

    // Avoid framebuffers with size 0/0
    if (0 == width_ || 0 == height_)
    {
        return false;
    }

    FramebufferDescriptor fbdesc;

    if (fbChildren)
    {
        fbdesc = fbChildren->getDescriptor();
        if (fbdesc.width == fbWidth && fbdesc.height == fbHeight)
        {
            return true;
        }

        releaseFramebuffers(dataStore_, commandBuffer_);

        fbdesc.width  = fbWidth;
        fbdesc.height = fbHeight;
    }
    else
    {
        fbdesc         = fbParent_->getDescriptor();
        fbdesc.width   = fbWidth;
        fbdesc.height  = fbHeight;
        fbdesc.flags   = gtf::scdr::FramebufferFlags::ColorBufferAsTexture;
        fbdesc.samples = 1;
    }

    return acquireFramebuffers(fbdesc, dataStore_, commandBuffer_);
}

auto GaussianBlurRenderObject::releaseFramebuffers(gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_) -> void
{
    BIND_GPU_CONTEXT(gc, dataStore_);

    gc.destroyFramebuffer(fbChildren, commandBuffer_);
    fbChildren = nullptr;

    gc.destroyFramebuffer(fbBlur[static_cast<uint32_t>(Pass::Horizontal)], commandBuffer_);
    gc.destroyFramebuffer(fbBlur[static_cast<uint32_t>(Pass::Vertical)], commandBuffer_);

    memset(fbBlur, 0, sizeof(fbBlur));
}

auto GaussianBlurRenderObject::acquireFramebuffers(const gtf::scdr::FramebufferDescriptor& fbdesc_, gtf::scdr::DataStore& dataStore_, gtf::scdr::CommandBuffer& commandBuffer_) -> bool
{
    BIND_GPU_CONTEXT(gc, dataStore_);

    fbChildren = gc.createFramebuffer(fbdesc_, commandBuffer_);
    if (!fbChildren)
    {
        return false;
    }

    if (effectUniforms.enabled && api == gtf::scdr::GraphicsApi::OpenGLES3)
    {
        fbBlur[static_cast<uint32_t>(Pass::Vertical)] = gc.createFramebuffer(fbChildren);

        FramebufferDescriptor fbHorizontal = fbdesc_;
        fbHorizontal.depthStencilFormat    = gtf::scdr::TextureFormat::Unknown;

        fbBlur[static_cast<uint32_t>(Pass::Horizontal)] = gc.createFramebuffer(fbHorizontal, commandBuffer_);
    }

    return true;
}

static float EvalGauss(float sigma_, float x_)
{
    //
    // Note: We knowingly ignore the normalization factor here, because
    // we later re-normalize the weights anyway.
    //
    const auto t1 = sigma_ > 0.f ? -((x_ * x_) / (2.f * sigma_ * sigma_)) : 0.f;
    return expf(t1);
}

static uint32_t ComputeWeightCount(float sigma_)
{
    //
    // Rule of thumb:
    //
    // The number weights we want to use until the contribution of a single weight becomes negligible
    // is about 3 * sigma
    //
    //
    // The exact formula for the distance x (number of weights)
    // at which the normal distribution falls below a given threshold T is given by:
    //
    // x >= sqrt(-2*sigma*ln(T * sqrt(2*PI) * sigma))
    //
    uint32_t iWeightCnt = static_cast<uint32_t>(ceilf(3.f * sigma_));
    iWeightCnt          = iWeightCnt < 1 ? 1 : iWeightCnt;

    // ensure that the return value is odd
    iWeightCnt += ((iWeightCnt + 1) % 2);

    return iWeightCnt;
}

float GetCombinedWeight(float w0_, float w1_)
{
    return w0_ + w1_;
}

float AdjustOffset(float offset0_, float offset1_, float w0_, float w1_, float combinedWeight_)
{
    //
    // see http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
    //
    return (offset0_ * w0_ + offset1_ * w1_) / combinedWeight_;
}

static void NormalizeWeights(uint32_t count_, float* weights_)
{
    float sum = weights_[0];
    for (uint32_t i = 1; i < count_; ++i)
    {
        sum += 2 * weights_[i];
    }

    for (uint32_t i = 0; i < count_; ++i)
    {
        weights_[i] /= sum;
    }
}

static uint32_t ComputeWeights(const GaussianBlurWeightParams& params_, float weights_[MAX_FILTER_WEIGHT_COUNT], float offsetx_[MAX_FILTER_WEIGHT_COUNT], float offsety_[MAX_FILTER_WEIGHT_COUNT])
{
    const auto fWidth      = static_cast<float>(params_.width);
    const auto fHeight     = static_cast<float>(params_.height);
    const auto sigma       = params_.sigma;

    const auto weightCount = ComputeWeightCount(sigma);
    GTF_OSAL_ASSERT(weightCount % 2 == 1);

    offsetx_[0] = 0.f;
    offsety_[0] = 0.f;
    weights_[0] = EvalGauss(sigma, 0.f);

    if (fWidth == 0.f || fHeight == 0.f)
        return weightCount;

    uint32_t linearSampledWeightCount = (weightCount - 1) / 2 + 1;
    linearSampledWeightCount = linearSampledWeightCount > (MAX_FILTER_WEIGHT_COUNT - 1) ? (MAX_FILTER_WEIGHT_COUNT - 1) : linearSampledWeightCount;

    for (uint32_t i = 1; i < linearSampledWeightCount; ++i)
    {
        const auto i0         = static_cast<float>(2 * i - 1);
        const auto i1         = static_cast<float>(2 * i);
        const auto offsetX0   = i0 / fWidth;
        const auto offsetX1   = i1 / fWidth;
        const auto offsetY0   = i0 / fHeight;
        const auto offsetY1   = i1 / fHeight;
        const auto weight0    = EvalGauss(sigma, i0);
        const auto weight1    = EvalGauss(sigma, i1);
        const auto combWeight = GetCombinedWeight(weight0, weight1);
        const auto adjOffsetX = AdjustOffset(offsetX0, offsetX1, weight0, weight1, combWeight);
        const auto adjOffsetY = AdjustOffset(offsetY0, offsetY1, weight0, weight1, combWeight);

        weights_[i] = combWeight;
        offsetx_[i] = adjOffsetX;
        offsety_[i] = adjOffsetY;
    }

    NormalizeWeights(linearSampledWeightCount, weights_);

    return linearSampledWeightCount;
}


void GaussianBlurRenderObject::setWeights(const GaussianBlurWeightParams& params_)
{
    effectUniforms.weightCount = ComputeWeights(params_, effectUniforms.weights, effectUniforms.offsetx, effectUniforms.offsety);
    effectUniforms.sigma       = params_.sigma;

    currentWeightParams = params_;
}

auto GaussianBlurRenderObject::setDefaultWeights() -> void
{
    const GaussianBlurWeightParams defaultParams = makeDefaultParams();

    setWeights(defaultParams);
}

auto GaussianBlurRenderObject::updateWeights(float sigma_) -> void
{
    const auto colorDesc = fbChildren->getDescriptor();

    const GaussianBlurWeightParams newParams = makeParams(colorDesc.width, colorDesc.height, sigma_);
    
    if (newParams != currentWeightParams)
    {
        setWeights(newParams);
    }
}
