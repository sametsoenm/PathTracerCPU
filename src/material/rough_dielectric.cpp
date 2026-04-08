#include "rough_dielectric.h"

#include "util/sampling.h"
#include "util/onb.h"
#include "material_funcs.h"
#include <iostream>

BSDFSample RoughDielectricMaterial::sample(
    const glm::vec3& wo, const Intersection& rec, const glm::vec2& u) const {

    BSDFSample s{};

    float etaI = 1.0f; // air IOR
    float etaJ = _eta;
    if (!rec.front_face) {
        etaI = _eta;
        etaJ = 1.0f;
    }

    Onb onb(rec.normal);
    const glm::vec3 wo_local = onb.to_local(wo);

    const float alpha = _alphaTex.sample(rec.uv.x, rec.uv.x).x;
    // need one more random number!!! --------------------------------------------------------------------------
    const glm::vec3 wh_local = material::sampleGGXVNDF(alpha, wo_local, u);

    const float cosr = glm::max(0.0f, glm::dot(wo_local, wh_local));
    const float eta = etaI / etaJ;
    const float sint2 = eta * eta * (1.0f - cosr * cosr);
    const float cost = glm::sqrt(1.0f - sint2);

    float F = 0.0f;
    // check for total internal reflection
    if (sint2 >= 1.0f) {
        s.sign = -1.0f; 
        F = 1.0f;
    }
    else {
        F = material::fresnel_dielectric(etaI, etaJ, cosr, cost);
    }

    // reflection
    if (u.x < F) {
        
        const glm::vec3 wi_local = glm::reflect(-wo_local, wh_local);

        const float G1 = material::G1_smith(alpha, wo_local, wh_local);
        const float D = material::NDF_GGX(alpha, wh_local);
        const float wo_dot_wh = glm::dot(wo_local, wh_local);
        const float wo_dot_wh_clamp = glm::max(0.0f, wo_dot_wh);

        glm::vec3 wi = onb.to_world(wi_local);
        s.wi = wi;
        s.pdf = G1 * glm::abs(wo_dot_wh) * D / (4.0f * glm::abs(wo_dot_wh) * wo_local.z) * F;

        // bsdf_cos
        if (wi_local.z <= 0.0f) {
            s.pdf = 0.0f;
            s.contrib = glm::vec3(0.0f);
            return s;
        }

        const float G = material::G_smith(alpha, wi_local, wo_local, wh_local);
        //const float bsdf_cos = F * D * G / (4.0f * wo_local.z);
        //s.contrib = glm::vec3(bsdf_cos / s.pdf);
        
        s.contrib = glm::vec3(F * G * wo_dot_wh / (G1 * wo_dot_wh_clamp));
        return s;
    }

    // transmission
    const float T = 1.0f - F;
    s.sign = -1.0f;

    const glm::vec3 wi_local = material::refract(-wo_local, wh_local, etaI, etaJ);

    if (wo_local.z * wi_local.z > 0.0f) { // check same hemisphere
        s.pdf = 0.0f;
        s.contrib = glm::vec3(0.0f);
    }

    const float G1 = material::G1_smith(alpha, wo_local, wh_local);
    const float D = material::NDF_GGX(alpha, wh_local);
    const float wo_dot_wh = glm::dot(wo_local, wh_local);
    const float wi_dot_wh = glm::dot(wi_local, wh_local); // abs?
    const float wo_dot_wh_clamp = glm::max(0.0f, wo_dot_wh);

    const float denom_sqrt = wi_dot_wh + wo_dot_wh / (etaJ / etaI); 
    const float jacobian = glm::abs(wo_dot_wh) / (denom_sqrt * denom_sqrt);

    s.pdf = G1 * glm::abs(wo_dot_wh) * D / glm::abs(wo_local.z) * jacobian * T;

    const float G = material::G_smith(alpha, wi_local, wo_local, wh_local);
    float bsdf_cos = T * D * G * glm::abs(wi_dot_wh) * glm::abs(wo_dot_wh) 
                         / (denom_sqrt * denom_sqrt * glm::abs(wo_local.z));

    bsdf_cos *= etaJ * etaJ / etaI / etaI;

    glm::vec3 wi = onb.to_world(wi_local);
    s.wi = wi;
    s.contrib = glm::vec3(bsdf_cos / s.pdf);
    return s;
}


float RoughDielectricMaterial::pdf(
    const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const {

    float etaI = 1.0f; // air IOR
    float etaJ = _eta;
    if (!rec.front_face) {
        etaI = _eta;
        etaJ = 1.0f;
    }

    const glm::vec3 N = rec.normal;
    Onb onb(N);
    const glm::vec3 wo_local = onb.to_local(wo);
    const glm::vec3 wi_local = onb.to_local(wi);

    const float alpha = _alphaTex.sample(rec.uv.x, rec.uv.x).x;
    const bool reflect = wo_local.z * wi_local.z > 0.0f;

    if (reflect) {
        glm::vec3 wh_local = glm::normalize(wo_local + wi_local);
        const float G1 = material::G1_smith(alpha, wo_local, wh_local);
        const float D = material::NDF_GGX(alpha, wh_local);
        const float wo_dot_wh = glm::dot(wo_local, wh_local);
        const float wo_dot_wh_clamp = glm::max(0.0f, wo_dot_wh);

        const float cosr = glm::max(0.0f, glm::dot(wo_local, wh_local));
        const float eta = etaI / etaJ;
        const float sint2 = eta * eta * (1.0f - cosr * cosr);
        const float cost = glm::sqrt(1.0f - sint2);
        const float F = sint2 < 1.0f
            ? material::fresnel_dielectric(etaI, etaJ, cosr, cost) : 1.0f;

        return G1 * wo_dot_wh_clamp * D / (4.0f * wo_dot_wh * wo_local.z) * F;

    }

    // transmission
    glm::vec3 wh_local = glm::normalize((etaJ * wi_local + etaI * wo_local)); // not 100% sure but probably correct

    if (dot(wo_local, wh_local) <= 0.0f)
        return 0.0f; // correct?

    const float cosr = glm::max(0.0f, glm::dot(wo_local, wh_local));
    const float eta = etaI / etaJ;
    const float sint2 = eta * eta * (1.0f - cosr * cosr);
    const float cost = glm::sqrt(1.0f - sint2);
    const float F = sint2 < 1.0f
        ? material::fresnel_dielectric(etaI, etaJ, cosr, cost) : 1.0f;

    const float G1 = material::G1_smith(alpha, wo_local, wh_local);
    const float D = material::NDF_GGX(alpha, wh_local);
    const float wo_dot_wh = glm::dot(wo_local, wh_local);
    const float wi_dot_wh = glm::dot(wi_local, wh_local); // abs?
    const float wo_dot_wh_clamp = glm::max(0.0f, wo_dot_wh);

    const float denom_sqrt = wi_dot_wh + wo_dot_wh / (etaJ / etaI);
    const float jacobian = glm::abs(wo_dot_wh) / (denom_sqrt * denom_sqrt);

    return (1.0f - F) * G1 * glm::abs(wo_dot_wh) * D / glm::abs(wo_local.z) * jacobian;

}

glm::vec3 RoughDielectricMaterial::eval_bsdf_cos(
    const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const {

    float etaI = 1.0f; // air IOR
    float etaJ = _eta;
    if (!rec.front_face) {
        etaI = _eta;
        etaJ = 1.0f;
    }

    const glm::vec3 N = rec.normal;
    Onb onb(N);
    const glm::vec3 wo_local = onb.to_local(wo);
    const glm::vec3 wi_local = onb.to_local(wi);

    const float alpha = _alphaTex.sample(rec.uv.x, rec.uv.x).x;
    const bool reflect = wo_local.z * wi_local.z > 0.0f;

    if (reflect) {
        glm::vec3 wh_local = glm::normalize(wo_local + wi_local);

        const float cosr = glm::max(0.0f, glm::dot(wo_local, wh_local));
        const float eta = etaI / etaJ;
        const float sint2 = eta * eta * (1.0f - cosr * cosr);
        const float cost = glm::sqrt(1.0f - sint2);
        const float F = sint2 < 1.0f 
            ? material::fresnel_dielectric(etaI, etaJ, cosr, cost) : 1.0f;

        const float wo_dot_wh = glm::max(0.0f, glm::dot(wo_local, wh_local));
        const float D = material::NDF_GGX(alpha, wh_local);
        const float G = material::G_smith(alpha, wi_local, wo_local, wh_local);

        return glm::vec3(F * D * G / (4.0f * wo_local.z));
    }

    // trnasmission
    glm::vec3 wh_local = glm::normalize((etaJ * wi_local + etaI * wo_local)); // not 100% sure but probably correct

    if (dot(wo_local, wh_local) <= 0.0f) 
        return glm::vec3(0.0f); // correct?

    const float cosr = glm::max(0.0f, glm::dot(wo_local, wh_local));
    const float eta = etaI / etaJ;
    const float sint2 = eta * eta * (1.0f - cosr * cosr);
    const float cost = glm::sqrt(1.0f - sint2);
    const float F = sint2 < 1.0f
        ? material::fresnel_dielectric(etaI, etaJ, cosr, cost) : 1.0f;
    const float D = material::NDF_GGX(alpha, wh_local);
    const float G = material::G_smith(alpha, wi_local, wo_local, wh_local);
    const float wo_dot_wh = glm::dot(wo_local, wh_local);
    const float wi_dot_wh = glm::dot(wi_local, wh_local); // abs?
    const float wo_dot_wh_clamp = glm::max(0.0f, wo_dot_wh);

    const float denom_sqrt = wi_dot_wh + wo_dot_wh / (etaJ / etaI);

    float bsdf_cos = (1.0f - F) * D * G * glm::abs(wi_dot_wh) * glm::abs(wo_dot_wh)
                   / (denom_sqrt * denom_sqrt * glm::abs(wo_local.z));
    bsdf_cos *= etaJ * etaJ / etaI / etaI;

    return glm::vec3(bsdf_cos);
}
