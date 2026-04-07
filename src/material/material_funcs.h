#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>

#include "core/intersection.h"


namespace material {

	inline glm::vec3 mapNormal(const Intersection& rec, const Texture& tex) {
		const glm::vec3 N_s = rec.shading_normal;
		const glm::vec3 T = glm::normalize(rec.tangent - N_s * glm::dot(rec.tangent, N_s)); // orthogonalize
		const glm::vec3 B = -glm::cross(N_s, T); // handedness??

		glm::vec3 N_ts = tex.sample(rec.uv.x, rec.uv.y);
		N_ts = 2.0f * N_ts - glm::vec3(1.0f);
		N_ts = glm::normalize(N_ts);

		glm::vec3 N_ws = glm::normalize(T * N_ts.x + B * N_ts.y + N_s * N_ts.z);
		//N_ws = rec.front_face ? N_ws : -N_ws; // ?
		return N_ws;
	}

	inline glm::vec3 refract(
		const glm::vec3& I, const glm::vec3& N, const float etaI, const float etaT) {
		const float eta = etaI / etaT;
		const float k = 1.f - eta * eta * (1.f - glm::dot(N, I) * glm::dot(N, I));
		return k < 0.f ? glm::vec3(0.f) : eta * I - (eta * dot(N, I) + glm::sqrt(k)) * N;
	}

	inline glm::vec3 fresnel_schlick(const glm::vec3& f0, const float cos_theta) {
		return f0 + (1.0f - f0) * glm::pow((1.0f - cos_theta), 5.0f);
	}

	inline float fresnel(
		const std::complex<float> N1, const std::complex<float> N2, const float cosr) {
		const auto cost = std::sqrt(1.0f - N1 * N1 / N2 / N2 * (1.0f - cosr * cosr));

		const auto Rs = std::abs((N1 * cosr - N2 * cost) / (N1 * cosr + N2 * cost));
		const auto Rp = std::abs((N1 * cost - N2 * cosr) / (N1 * cost + N2 * cosr));
		return (Rs * Rs + Rp * Rp) * 0.5f;
	}

	inline glm::vec3 fresnel(
		const glm::vec3& eta1, const glm::vec3& kappa1,
		const glm::vec3& eta2, const glm::vec3& kappa2, const float cos_theta) {

		return glm::vec3(
			fresnel({ eta1.x, kappa1.x }, { eta2.x, kappa2.x }, cos_theta),
			fresnel({ eta1.y, kappa1.y }, { eta2.y, kappa2.y }, cos_theta),
			fresnel({ eta1.z, kappa1.z }, { eta2.z, kappa2.z }, cos_theta)
		);
	}

	inline glm::vec3 fresnel(
		const glm::vec3& eta, const glm::vec3& kappa, const float cos_theta) {

		return glm::vec3(
			fresnel(1.0f, { eta.x, kappa.x }, cos_theta),
			fresnel(1.0f, { eta.y, kappa.y }, cos_theta),
			fresnel(1.0f, { eta.z, kappa.z }, cos_theta)
		);
	}

	inline float fresnel_dielectric(
		const float eta1, const float eta2, const float cosr, const float cost) {

		const auto Rs = (eta1 * cosr - eta2 * cost) / (eta1 * cosr + eta2 * cost);
		const auto Rp = (eta1 * cost - eta2 * cosr) / (eta1 * cost + eta2 * cosr);
		return (Rs * Rs + Rp * Rp) * 0.5f;
	}

	inline float NDF_GGX(const float alpha, const glm::vec3& wh) {
		if (wh.z <= 0.0f) // cos_theta assuming N is (0,0,1)
			return 0.0f;
		const float a2 = alpha * alpha;
		const float denom_base = wh.x * wh.x + wh.y * wh.y + a2 * wh.z * wh.z;
		return a2 / (math::PI * denom_base * denom_base);
	}

	inline float G1_smith(const float alpha, const glm::vec3& wx, const glm::vec3& wh) {
		if (glm::dot(wx, wh) <= 0.0f)
			return 0.0f;

		const float a2 = alpha * alpha;
		const float frac = (wx.x * wx.x + wx.y * wx.y) / (wx.z * wx.z);
		const float lambda = (-1.0f + glm::sqrt(1 + a2 * frac)) * 0.5f;
		return 1.0f / (1.0f + lambda);
	}

	inline float G_smith(const float alpha,
		const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& wh) {
		return G1_smith(alpha, wi, wh) * G1_smith(alpha, wo, wh);
	}

	static glm::vec3 sampleGGXVNDFHemisphere(const glm::vec2& u, const glm::vec3& wo) {
		// orthonormal basis (with special case if cross product is 0)
		const float tmp = wo.x * wo.x + wo.y * wo.y;
		const glm::vec3 w1 = tmp > 0.0f
			? glm::vec3(-wo.y, wo.x, 0.0f) * glm::inversesqrt(tmp)
			: glm::vec3(1.0f, 0.0f, 0.0f);
		const glm::vec3 w2 = glm::cross(wo, w1);
		// parameterization of the projected area
		const float r = glm::sqrt(u.x);
		const float phi = 2.f * static_cast<float>(math::PI) * u.y;
		const float t1 = r * glm::cos(phi);
		float t2 = r * glm::sin(phi);
		const float s = (1.0f + wo.z) / 2.0f;
		t2 = (1.0f - s) * glm::sqrt(1.0f - t1 * t1) + s * t2;
		const float ti = glm::sqrt(glm::max(1.0f - t1 * t1 - t2 * t2, 0.0f));
		// reprojection onto hemisphere
		const glm::vec3 wm = t1 * w1 + t2 * w2 + ti * wo;
		// return hemispherical sample
		return wm;
	}

	inline glm::vec3 sampleGGXVNDF(
		const float alpha, const glm::vec3& wo, const glm::vec2& u) {
		const glm::vec3 woStd = glm::normalize(glm::vec3(wo.x * alpha, wo.y * alpha, wo.z));
		// sample the hemisphere
		const glm::vec3 wmStd = sampleGGXVNDFHemisphere(u, woStd);
		// warp back to the ellipsoid configuration
		const glm::vec3 wm = glm::normalize(
			glm::vec3(wmStd.x * alpha, wmStd.y * alpha, glm::max(0.f, wmStd.z))
		);
		// return final normal
		return wm;
	}

}