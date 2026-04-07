#include "specular_microfacet.h"

#include "util/sampling.h"
#include "util/onb.h"
#include "material_funcs.h"

BSDFSample SpecularMicrofacetMaterial::sample(
	const glm::vec3& wo, const Intersection& rec, const glm::vec2& u) const {

    BSDFSample s{};

    Onb onb(rec.normal);
    const glm::vec3 wo_local = onb.to_local(wo);
    const float alpha = _alphaTex.sample(rec.uv.x, rec.uv.y).x;
    const glm::vec3 wh_local = material::sampleGGXVNDF(alpha, wo_local, u);
    const glm::vec3 wi_local = glm::reflect(-wo_local, wh_local);

    const float G1 = material::G1_smith(alpha, wo_local, wh_local);
    const float D = material::NDF_GGX(alpha, wh_local);
    const float wo_dot_wh = glm::dot(wo_local, wh_local);
    const float wo_dot_wh_clamp = glm::max(0.0f, wo_dot_wh);

    s.pdf = G1 * wo_dot_wh_clamp * D / (4.0f * wo_dot_wh * wo_local.z);

    glm::vec3 wi = onb.to_world(wi_local);

    s.is_delta = false;
    s.wi = wi;
    s.contrib = eval_bsdf_cos(wo, wi, rec) / s.pdf;

    return s;
}

float SpecularMicrofacetMaterial::pdf(
	const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const {

    Onb onb(rec.normal);
    const glm::vec3 wo_local = onb.to_local(wo);
    const glm::vec3 wi_local = onb.to_local(wi);
    const glm::vec3 wh_local = glm::normalize(wo_local + wi_local);

    const float alpha = _alphaTex.sample(rec.uv.x, rec.uv.y).x;
    const float G1 = material::G1_smith(alpha, wo_local, wh_local);
    const float D = material::NDF_GGX(alpha, wh_local);
    const float wo_dot_wh = glm::dot(wo_local, wh_local);
    const float wo_dot_wh_clamp = glm::max(0.0f, wo_dot_wh);

    return G1 * wo_dot_wh_clamp * D / (4.0f * wo_dot_wh * wo_local.z);

}

glm::vec3 SpecularMicrofacetMaterial::eval_bsdf_cos(
	const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const {

    const glm::vec3 N = _hasNormalMap
        ? material::mapNormal(rec, _normalTex)
        : rec.shading_normal;
    Onb onb(N);
    const glm::vec3 wo_local = onb.to_local(wo);
    const glm::vec3 wi_local = onb.to_local(wi);
    
    if (wo_local.z <= 0.0f || wi_local.z <= 0.0f)
        return glm::vec3(0.0f);

    const glm::vec3 wh_local = glm::normalize(wo_local + wi_local);
    const float wo_dot_wh = glm::max(0.0f, glm::dot(wo_local, wh_local));

    glm::vec3 f0 = _f0Tex.sample(rec.uv.x, rec.uv.y);
    const float alpha = _alphaTex.sample(rec.uv.x, rec.uv.y).x;
    const glm::vec3 F = material::fresnel_schlick(f0, wo_dot_wh);
    //const glm::vec3 F = material::fresnel(_eta, _kappa, wo_dot_wh);
    const float D = material::NDF_GGX(alpha, wh_local);
    const float G = material::G_smith(alpha, wi_local, wo_local, wh_local);

    const float V = glm::dot(rec.normal, wi) <= 0.0f ? 0.0f : 1.0f; // necessary if shading normal differs from geometric normal

    return V * F * D * G / (4.0f * wo_local.z);

}
