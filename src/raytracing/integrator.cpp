#include <raytracing/integrator.h>


Integrator::Integrator():
    max_depth(5)
{
    scene = NULL;
    intersection_engine = NULL;
}

//Basic ray trace
glm::vec3 Integrator::TraceRay(Ray r, unsigned int depth)
{
    if(depth > max_depth)
        return glm::vec3(0,0,0);

    Intersection intersection = intersection_engine->GetIntersection(r);
    if(intersection.t > 0)
    {
        if(intersection.object_hit->material->emissive)
            return intersection.color;

        glm::vec3 resultColor(0.0f);

        for(Geometry* light : scene->lights)
        {
            glm::vec3 reflectedColor(1.0f);
            glm::vec3 localIlluColor(1.0f);

            if(intersection.object_hit->material->reflectivity > 0 ) // reflective material
            {
                glm::vec3 reflectDirection(glm::normalize(glm::reflect(r.direction, intersection.normal)));
                Ray reflectedRay(intersection.point + float(1e-3)*reflectDirection, reflectDirection);
                reflectedColor = TraceRay(reflectedRay, depth+1) * intersection.color;
            }

            if(!fequal(intersection.object_hit->material->reflectivity, 1.0f)) // not fully mirrored
            {
                if(intersection.object_hit->material->refract_idx_in != 0) // refractive material
                {
                    float eta = 1.0f;
                    int p=1;
                    if(glm::dot(r.direction,intersection.normal) < 0) //entering object
                    {
                        p = -1;
                        eta = intersection.object_hit->material->refract_idx_out / intersection.object_hit->material->refract_idx_in;
                    }
                    else  // leaving object
                    {
                        eta = intersection.object_hit->material->refract_idx_in / intersection.object_hit->material->refract_idx_out;
                    }

                    glm::vec3 refractDirection(glm::normalize(glm::refract(r.direction, float(-p)*intersection.normal, eta)));
                    if(!isnan(refractDirection.x) && !isnan(refractDirection.y) && !isnan(refractDirection.z))  //critical angle
                    {
                        Ray refractedRay(intersection.point + p*float(1e-3)*intersection.normal, refractDirection);
                        localIlluColor = TraceRay(refractedRay, depth+1);
                    }
                    else // fully reflected!
                    {
                        //localIlluColor = glm::vec3()
                        glm::vec3 fullReflectedDirection(glm::normalize(glm::reflect(r.direction, float(-p)*intersection.normal)));
                        Ray fullReflectedRay(intersection.point + p*float(1e-3)*intersection.normal, fullReflectedDirection);
                        localIlluColor = TraceRay(fullReflectedRay, depth+1);
                    }
                    localIlluColor = localIlluColor * intersection.color;
                }
               else
               {

                    localIlluColor = localIlluColor * intersection.object_hit->material->EvaluateReflectedEnergy(intersection,
                                                                                                light->transform.position()-intersection.point,
                                                                                                r.direction);

                    localIlluColor = localIlluColor * ShadowTest(intersection.point + float(1e-3)*intersection.normal, light);
                }
            }

            glm::vec3 surfaceColor =  (1.0f-intersection.object_hit->material->reflectivity) * localIlluColor +
                    intersection.object_hit->material->reflectivity * reflectedColor;
            resultColor = resultColor + surfaceColor * light->material->base_color;
        }

        return resultColor / float(scene->lights.size());
    }
    else
        return glm::vec3(0,0,0);
}

// shadow test
glm::vec3 Integrator::ShadowTest(glm::vec3 point, Geometry* light)
{
    Intersection intersection = intersection_engine->GetIntersection(Ray(point,light->transform.position()-point));

    if(intersection.t < 0) // hit nothing??
        return glm::vec3(1,1,1);
    else
    {
//        ///***********following are soft shadow test part*************
//        glm::vec3 sumColor(0);
//        Ray point_light= Ray(point,light->transform.position()-point);
//        Ray object_space_ray = point_light.GetTransformedCopy(light->transform.invT());
//        Intersection onObjectHit = light->GetIntersection(object_space_ray);
//        if(onObjectHit.t > 0)
//        {
//            light->SamplingPointsOnSurface(onObjectHit.point);

//            for(glm::vec3 sp:light->SamplePoints)
//            {
//               Intersection temp = intersection_engine->GetIntersection(Ray(point,sp-point));
//               if(temp.object_hit == light)
//                   sumColor = sumColor + light->material->base_color;
//            }
//        }
//        return sumColor / float(light->SamplePoints.size());
//        ///**************************************************************/


        ///***************Normal shadow test**************************
        if(intersection.object_hit->material->emissive)
            return intersection.color;
        else if(intersection.object_hit->material->refract_idx_in != 0)
        {
            Ray r(intersection.point + float(1e-3)*(light->transform.position()-intersection.point),
                  light->transform.position()-intersection.point);
            return intersection.color * ShadowTest(r.origin, light);
        }
        else
        {
            return glm::vec3(0.2f);
        }
        ///**************************************************************
    }



}

void Integrator::SetDepth(unsigned int depth)
{
    max_depth = depth;
}
