#include "scene.h"

#include "material/mirror.h"
#include "material/diffuse_emissive.h"
#include "material/specular_microfacet.h"
#include "material/dielectric.h"
#include "material/rough_dielectric.h"

#include "intersectables/bvh.h"

Scene::Scene(settings::SceneType type) {
    loadEnvMap(settings::ENV_MAP_ON_START);
    switch (type) {
    case settings::SceneType::CORNELL_BOX:
        cornell_box_scene();
        break;
    case settings::SceneType::SPHERES:
        sphere_scene();
        break;
    case settings::SceneType::OG:
        og_scene();
        break;
    case settings::SceneType::SUZANNE:
        suzanne_scene();
        break;
    case settings::SceneType::EMPTY:
        break;
    default:
        break;
    }

    buildBvh();
    //useBVH(false);
}

void Scene::buildBvh() {

    if (_objects.objects().empty()) {
        _world = nullptr;
        return;
    }

    //make copy for bvh
    std::vector<std::shared_ptr<Intersectable>> objects = _objects.objects();
    _world = std::make_shared<BVHNode>(objects, 0, objects.size());
}

void Scene::suzanne_scene() {

    // mats
    auto white_diffuse = make_shared<LambertDiffuseMaterial>(glm::vec3(1.0f));
    auto red_diffuse = make_shared<LambertDiffuseMaterial>(glm::vec3(1.0f, 0.0f, 0.0f));
    auto green_diffuse = make_shared<LambertDiffuseMaterial>(glm::vec3(0.0f, 1.0f, 0.0f));
    auto light = make_shared<DiffuseEmissiveMaterial>(glm::vec3(15.f));
    auto glass = make_shared<DielectricMaterial>();
    auto metal = make_shared<SpecularMicrofacetMaterial>(material::metal::F0_IRON, 0.08f);
    auto metal2 = make_shared<SpecularMicrofacetMaterial>(material::metal::F0_COPPER, 0.28f);
    auto mirror = make_shared<MirrorMaterial>();
    auto textured = make_shared<SpecularMicrofacetMaterial>(
        Texture("assets/textures/metal.png"),
        Texture("assets/textures/roughness.png"));
    auto roughD = make_shared<RoughDielectricMaterial>(1.45f, Texture(glm::vec3(0.2f)));

    // cam
    _cam.setPosition(glm::vec3(0.0f, 0.0f, 2.5f));

    // light

    auto light1 = make_shared<Triangle>(
        glm::vec3(-0.5f, 1.49f, -0.5f),
        glm::vec3( 0.5f, 1.49f,  0.5f),
        glm::vec3(-0.5f, 1.49f,  0.5f),
        light);

    auto light2 = make_shared<Triangle>(
        glm::vec3(-0.5f, 1.49f, -0.5f),
        glm::vec3( 0.5f, 1.49f, -0.5f),
        glm::vec3( 0.5f, 1.49f,  0.5f),
        light);
    _lights.add(light1);
    _lights.add(light2);
    _objects.add(light1);
    _objects.add(light2);


    // FLOOR (y = -0.5)
    _objects.add(make_shared<Triangle>(
        glm::vec3(-1.5f, -1.5f, -1.5f),
        glm::vec3(-1.5f, -1.5f,  1.5f),
        glm::vec3( 1.5f, -1.5f,  1.5f),
        white_diffuse));
    _objects.add(make_shared<Triangle>(
        glm::vec3(-1.5f, -1.5f, -1.5f),
        glm::vec3( 1.5f, -1.5f,  1.5f),
        glm::vec3( 1.5f, -1.5f, -1.5f),
        white_diffuse));

    // CEILING (y = 0.5)
    _objects.add(make_shared<Triangle>(
        glm::vec3(-1.5f, 1.5f, -1.5f),
        glm::vec3( 1.5f, 1.5f,  1.5f),
        glm::vec3(-1.5f, 1.5f,  1.5f),
        white_diffuse));
    _objects.add(make_shared<Triangle>(
        glm::vec3(-1.5f, 1.5f, -1.5f),
        glm::vec3( 1.5f, 1.5f, -1.5f),
        glm::vec3( 1.5f, 1.5f,  1.5f),
        white_diffuse));

    // BACK WALL (z = -0.5)
    _objects.add(make_shared<Triangle>(
        glm::vec3(-1.5f, -1.5f, -1.5f),
        glm::vec3( 1.5f,  1.5f, -1.5f),
        glm::vec3(-1.5f,  1.5f, -1.5f),
        white_diffuse));
    _objects.add(make_shared<Triangle>(
        glm::vec3(-1.5f, -1.5f, -1.5f),
        glm::vec3( 1.5f, -1.5f, -1.5f),
        glm::vec3( 1.5f,  1.5f, -1.5f),
        white_diffuse));

    // LEFT WALL (x = -0.5)
    _objects.add(make_shared<Triangle>(
        glm::vec3(-1.5f, -1.5f, -1.5f),
        glm::vec3(-1.5f, 1.5f,  1.5f),
        glm::vec3(-1.5f, 1.5f, -1.5f),
        red_diffuse));
    _objects.add(make_shared<Triangle>(
        glm::vec3(-1.5f, -1.5f, -1.5f),
        glm::vec3(-1.5f, -1.5f,  1.5f),
        glm::vec3(-1.5f,  1.5f,  1.5f),
        red_diffuse));

    // RIGHT WALL (x = 0.5)
    _objects.add(make_shared<Triangle>(
        glm::vec3(1.5f, -1.5f, -1.5f),
        glm::vec3(1.5f,  1.5f, -1.5f),
        glm::vec3(1.5f,  1.5f,  1.5f),
        green_diffuse));
    _objects.add(make_shared<Triangle>(
        glm::vec3(1.5f, -1.5f, -1.5f),
        glm::vec3(1.5f,  1.5f,  1.5f),
        glm::vec3(1.5f, -1.5f,  1.5f),
        green_diffuse));


    load_obj("assets/models/suzanne.obj", roughD, glm::vec3(2.4f, -1.5f, -4.4f), glm::vec3(1.0f));
    //load_obj("assets/models/sponza.obj", white_diffuse, glm::vec3(0.0f), glm::vec3(1.0f));
    //load_obj("assets/models/teapot.obj", roughD, glm::vec3(0.f, -1.5f, 0.f), glm::vec3(.4f));
}

void Scene::og_scene() {

    auto light = make_shared<AreaLight>();
    _lights.add(light);

    /*auto lightm = make_shared<DiffuseEmissiveMaterial>(glm::vec3(15.0f));

    auto light1 = make_shared<Triangle>(
        glm::vec3(-0.1f, 0.4f, -0.1f),
        glm::vec3(-0.1f, 0.4f,  0.1f),
        glm::vec3( 0.1f, 0.4f,  0.1f),
        lightm);

    auto light2 = make_shared<Triangle>(
        glm::vec3(-0.1f, 0.4f, -0.1f),
        glm::vec3( 0.1f, 0.4f, -0.1f),
        glm::vec3( 0.1f, 0.4f,  0.1f),
        lightm);
    _lights.add(light1);
    _lights.add(light2);
    _objects.add(light1);
    _objects.add(light2);*/

    _cam.setPosition(glm::vec3(0.0f, 0.0f, 0.5f));

    auto metal = make_shared<SpecularMicrofacetMaterial>(glm::vec3(0.0f, 0.0f, 0.97f), 0.25f);
    auto mirror = make_shared<MirrorMaterial>();

	auto sphere1 = std::make_shared<Sphere>(0.5f, glm::vec3(0.55f, 0.0f, -1.0f),
        metal);
	auto sphere2 = std::make_shared<Sphere>(0.5f, glm::vec3(-0.55f, 0.0f, -1.0f),
		make_shared<LambertDiffuseMaterial>(glm::vec3(0.0f, 0.0f, 0.7f)));
	auto sphere3 = std::make_shared<Sphere>(200.f, glm::vec3(0.0f, -200.5f, -1.0f),
		make_shared<LambertDiffuseMaterial>(glm::vec3(1.0f, 0.4f, 0.8f)));
    _objects.add(sphere1);
    _objects.add(sphere2);
    _objects.add(sphere3);
}

void Scene::cornell_box_scene() {

	// mats
	auto white_diffuse = make_shared<LambertDiffuseMaterial>(glm::vec3(1.0f));
	auto red_diffuse   = make_shared<LambertDiffuseMaterial>(glm::vec3(1.0f, 0.0f, 0.0f));
    auto green_diffuse = make_shared<LambertDiffuseMaterial>(glm::vec3(0.0f, 1.0f, 0.0f));
    auto textured2 = make_shared<SpecularMicrofacetMaterial>(Texture(glm::vec3(0.6f)), Texture(glm::vec3(0.3f)), Texture("assets/textures/normal2.png"));
    auto textured3     = make_shared<LambertDiffuseMaterial>(Texture(glm::vec3(0.6f)), Texture("assets/textures/normal2.png"));
    auto textured      = make_shared<LambertDiffuseMaterial>(Texture("assets/textures/testarossa.jpg", true));
    auto mirror        = make_shared<MirrorMaterial>();
    auto metal         = make_shared<SpecularMicrofacetMaterial>(material::metal::F0_ALUMINIUM, 0.15f);
    auto light         = make_shared<DiffuseEmissiveMaterial>(glm::vec3(20.0f, 10.0f, 6.0f));
    auto glass         = make_shared<DielectricMaterial>();

    // cam
    _cam.setPosition(glm::vec3(0.0f, 0.0f, 1.0f));

    // light

    auto light1 = make_shared<Triangle>(
        glm::vec3(-0.2f, 0.49f, -0.2f),
        glm::vec3(0.2f, 0.49f, 0.2f),
        glm::vec3(-0.2f, 0.49f, 0.2f),
        light);

    auto light2 = make_shared<Triangle>(
        glm::vec3(-0.2f, 0.49f, -0.2f),
        glm::vec3(0.2f, 0.49f, -0.2f),
        glm::vec3(0.2f, 0.49f, 0.2f),
        light);
    _lights.add(light1);
    _lights.add(light2);
    _objects.add(light1);
    _objects.add(light2);

    auto light3 = make_shared<AreaLight>(
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(10.0f, 7.0f, 5.0f)
    );
    //_lights.add(light3);

    // FLOOR (y = -0.5)
    _objects.add(make_shared<Triangle>(
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f, 0.5f),
        glm::vec3(0.5f, -0.5f, 0.5f),
        white_diffuse));
    _objects.add(make_shared<Triangle>(
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, 0.5f),
        glm::vec3(0.5f, -0.5f, -0.5f),
        white_diffuse));

    // CEILING (y = 0.5)
    _objects.add(make_shared<Triangle>(
        glm::vec3(-0.5f, 0.5f, -0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(-0.5f, 0.5f, 0.5f),
        white_diffuse));
    _objects.add(make_shared<Triangle>(
        glm::vec3(-0.5f, 0.5f, -0.5f),
        glm::vec3(0.5f, 0.5f, -0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        white_diffuse));

    // BACK WALL (z = -0.5)
    _objects.add(make_shared<Triangle>(
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, 0.5f, -0.5f),
        glm::vec3(-0.5f, 0.5f, -0.5f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f),
        textured));
    _objects.add(make_shared<Triangle>(
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, 0.5f, -0.5f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        textured));

    // LEFT WALL (x = -0.5)
    _objects.add(make_shared<Triangle>(
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(-0.5f, 0.5f, 0.5f),
        glm::vec3(-0.5f, 0.5f, -0.5f),
        red_diffuse));
    _objects.add(make_shared<Triangle>(
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f, 0.5f),
        glm::vec3(-0.5f, 0.5f, 0.5f),
        red_diffuse));

    // RIGHT WALL (x = 0.5)
    _objects.add(make_shared<Triangle>(
        glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, 0.5f, -0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        green_diffuse));
    _objects.add(make_shared<Triangle>(
        glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, -0.5f, 0.5f),
        green_diffuse));

    _objects.add(make_shared<Sphere>(
        0.3f, glm::vec3(0.0f, -0.2f, 0.0f), metal
    ));
	
}

void Scene::sphere_scene() {

    // mats
    auto white_diffuse = make_shared<LambertDiffuseMaterial>(glm::vec3(1.0f));
    auto red_diffuse = make_shared<LambertDiffuseMaterial>(glm::vec3(1.0f, 0.0f, 0.0f));
    auto light = make_shared<DiffuseEmissiveMaterial>(glm::vec3(15.f));
    auto glass = make_shared<DielectricMaterial>();
    auto metal = make_shared<SpecularMicrofacetMaterial>(material::metal::F0_IRON, 0.08f);
    auto metal2 = make_shared<SpecularMicrofacetMaterial>(material::metal::F0_COPPER, 0.28f);
    auto mirror = make_shared<MirrorMaterial>();
    auto textured = make_shared<SpecularMicrofacetMaterial>(
        Texture("assets/textures/metal.png"), 
        Texture("assets/textures/roughness.png")); 
    auto textured2 = make_shared<SpecularMicrofacetMaterial>(
        Texture(glm::vec3(0.6f)),
        Texture(glm::vec3(0.2f)),
        Texture("assets/textures/normal2.png"));
    auto roughD = make_shared<RoughDielectricMaterial>(1.3f, Texture(glm::vec3(0.1f)));

    // cam
    _cam.setFOV(60.0f);
    _cam.setPosition(glm::vec3(0.3f, 1.0f, 1.0f));
    _cam.setLookat(glm::vec3(0.03f, -0.2f, 0.2f));

    // light

    auto light1 = make_shared<Triangle>(
        glm::vec3(-0.5f, 0.3f, 1.5f),
        glm::vec3(-0.5f, 0.7f, 1.5f),
        glm::vec3(0.5f, 0.3f, 1.5f),
        light);

    auto light2 = make_shared<Triangle>(
        glm::vec3(0.5f, 0.3f, 1.5f),
        glm::vec3(-0.5f, 0.7f, 1.5f),
        glm::vec3(0.5f, 0.7f, 1.5f),
        light);

    auto light3 = make_shared<Triangle>(
        glm::vec3(2.0f, 0.7f, 0.5f),
        glm::vec3(2.0f, 0.3f, 0.5f),
        glm::vec3(2.0f, 0.3f, -0.5f),
        light);

    auto light4 = make_shared<Triangle>(
        glm::vec3(-0.2f, 1.49f, -0.2f),
        glm::vec3(0.2f,  1.49f, 0.2f),
        glm::vec3(-0.2f, 1.49f, 0.2f),
        light);

    auto light5 = make_shared<Triangle>(
        glm::vec3(-0.2f, 1.49f, -0.2f),
        glm::vec3(0.2f,  1.49f, -0.2f),
        glm::vec3(0.2f,  1.49f, 0.2f),
        light);
    _lights.add(light1);
    _lights.add(light2);
    _lights.add(light3);
    _lights.add(light4);
    _lights.add(light5);
    _objects.add(light1);
    _objects.add(light2);
    _objects.add(light3);
    _objects.add(light4);
    _objects.add(light5);


    // FLOOR (y = -0.5)
    _objects.add(make_shared<Triangle>(
        glm::vec3(-100.5f, -0.5f, -100.5f),
        glm::vec3(-100.5f, -0.5f,  50.5f),
        glm::vec3( 100.5f, -0.5f,  50.5f),
        white_diffuse));
    _objects.add(make_shared<Triangle>(
        glm::vec3(-100.5f, -0.5f, -100.5f),
        glm::vec3( 100.5f, -0.5f,  50.5f),
        glm::vec3( 100.5f, -0.5f, -100.5f),
        white_diffuse));


    _objects.add(make_shared<Sphere>(
        0.3f, glm::vec3(-0.36f, -0.2f, -0.36f), textured
    ));
    _objects.add(make_shared<Sphere>(
        0.3f, glm::vec3(0.36f, -0.2f, -0.36f), metal
    ));
    _objects.add(make_shared<Sphere>(
        0.3f, glm::vec3(-0.36f, -0.2f, 0.36f), red_diffuse
    ));
    _objects.add(make_shared<Sphere>(
        0.3f, glm::vec3(0.36f, -0.2f, 0.36f), roughD
    ));

    //load_obj("assets/models/suzanne.obj", white_diffuse, glm::vec3(2.0f, -1.0f, -5.f), glm::vec3(1.0f));
    //load_obj("assets/models/teapot.obj", metal2, glm::vec3(0.f), glm::vec3(.2f));
}

void Scene::add_object(const std::shared_ptr<Intersectable>& obj) {
    _objects.add(obj);
    _world = nullptr;
}

void Scene::useBVH(const bool b) {
    if (_bvhEnabled == b)
        return;
    else if (_bvhEnabled && !b)
        _world = std::make_shared<IntersectableList>(_objects.objects());
    else
        buildBvh();
    _bvhEnabled = b;
}

void Scene::loadEnvMap(std::string name) {
    for (auto m : light::envMaps) {
        if (m.name == name)
            _environment = m;
    }
    // TODO: maybe add special treatment if not found
}
