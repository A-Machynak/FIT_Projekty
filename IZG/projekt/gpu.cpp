/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>
#include <iostream>
#include <cstring>

#define _LOG(msg) { std::cout << "[DEBUG]: " << msg << std::endl; }

struct Primitive {
  OutVertex v[3];
};

struct ClippedPrimitive {
  ClippedPrimitive() {} 
  Primitive t[2];
  uint8_t pCount;
};

struct EdgeFnValues {
  float e[3];         // e0, e1, e2
  float xMinMax[2];   // xMin/xMax
  float yMinMax[2];   // yMin/yMax
  float delta0[2];    // deltaX/deltaY
  float delta1[2];    // deltaX/deltaY
  float delta2[2];    // deltaX/deltaY
};

inline float calculateTriangleArea(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2) {
  return glm::abs((p0.x * (p1.y - p2.y) + p1.x * (p2.y - p0.y) + p2.x * (p0.y - p1.y)) / 2.0f);
}

void computeVertexID(InVertex &inVertex, const VertexArray &vao, uint32_t vertexIndex) {
  if(vao.indexBuffer) {
    if(vao.indexType == IndexType::UINT8) {
      inVertex.gl_VertexID = ((uint8_t*)vao.indexBuffer)[vertexIndex];
    } else if(vao.indexType == IndexType::UINT16) {
      inVertex.gl_VertexID = ((uint16_t*)vao.indexBuffer)[vertexIndex];
    } else /*if(vao.indexType == IndexType::UINT32)*/ {
      inVertex.gl_VertexID = ((uint32_t*)vao.indexBuffer)[vertexIndex];
    }
  } else {
    inVertex.gl_VertexID = vertexIndex;
  }
}

void readVertexAttributes(InVertex &inVertex, const VertexArray &vao) {
  for(uint32_t j = 0; j < maxAttributes; j++) {
    if(vao.vertexAttrib[j].type == AttributeType::EMPTY) {
      continue;
    }

    size_t bOffset = vao.vertexAttrib[j].offset + vao.vertexAttrib[j].stride * inVertex.gl_VertexID;

    if(vao.vertexAttrib[j].type == AttributeType::FLOAT) {
      inVertex.attributes[j].v1 = *((float*)(((char*)vao.vertexAttrib[j].bufferData + bOffset)));
    } else if(vao.vertexAttrib[j].type == AttributeType::VEC2) {
      inVertex.attributes[j].v2 = *((glm::vec2*)(((char*)vao.vertexAttrib[j].bufferData + bOffset)));
    } else if(vao.vertexAttrib[j].type == AttributeType::VEC3) {
      inVertex.attributes[j].v3 = *((glm::vec3*)(((char*)vao.vertexAttrib[j].bufferData + bOffset)));
    } else if(vao.vertexAttrib[j].type == AttributeType::VEC4) {
      inVertex.attributes[j].v4 = *((glm::vec4*)(((char*)vao.vertexAttrib[j].bufferData + bOffset)));
    }
  }
}

void perspectiveDivisionAndVPTransformation(Primitive &t, float width, float height) {
  for(int i = 0; i < 3; i++) {
    t.v[i].gl_Position.x = (t.v[i].gl_Position.x / t.v[i].gl_Position.w + 1.f) * (width / 2.f);
    t.v[i].gl_Position.y = (t.v[i].gl_Position.y / t.v[i].gl_Position.w + 1.f) * (height / 2.f);
    t.v[i].gl_Position.z /= t.v[i].gl_Position.w;
  }
}

bool isClockwiseWindingOrder(Primitive &t) {
  auto normal = glm::cross(
      glm::vec3(t.v[1].gl_Position.x - t.v[0].gl_Position.x, 
                t.v[1].gl_Position.y - t.v[0].gl_Position.y,
                0.f),
      glm::vec3(t.v[2].gl_Position.x - t.v[1].gl_Position.x, 
                t.v[2].gl_Position.y - t.v[1].gl_Position.y,
                0.f)
  );

  return normal.z < 0.0f;
}

/**
 * @brief Interpolate triangle attributes
 * 
 */
void interpolateAttributes(InFragment &inFrag, const Primitive &t, const Program &prg, float lambda0, float lambda1, float lambda2) {
  for(uint32_t j = 0; j < maxAttributes; j++) {
    if(prg.vs2fs[j] == AttributeType::EMPTY) {
      continue;
    }

    if(prg.vs2fs[j] == AttributeType::FLOAT) {
      inFrag.attributes[j].v1 = 
              t.v[0].attributes[j].v1 * lambda0
            + t.v[1].attributes[j].v1 * lambda1
            + t.v[2].attributes[j].v1 * lambda2;
    } else if(prg.vs2fs[j] == AttributeType::VEC2) {
      inFrag.attributes[j].v2 = 
              t.v[0].attributes[j].v2 * lambda0
            + t.v[1].attributes[j].v2 * lambda1
            + t.v[2].attributes[j].v2 * lambda2;
    } else if(prg.vs2fs[j] == AttributeType::VEC3) {
      inFrag.attributes[j].v3 = 
              t.v[0].attributes[j].v3 * lambda0
            + t.v[1].attributes[j].v3 * lambda1
            + t.v[2].attributes[j].v3 * lambda2;
    } else if(prg.vs2fs[j] == AttributeType::VEC4) {
      inFrag.attributes[j].v4 = 
              t.v[0].attributes[j].v4 * lambda0
            + t.v[1].attributes[j].v4 * lambda1
            + t.v[2].attributes[j].v4 * lambda2;
    }
  }
}

/**
 * @brief Calculate edge function values
 * 
 * @param efv output
 * @param t triangle
 * @param width max width
 * @param height max height
 */
void calculateEdgeFnValues(EdgeFnValues &efv, Primitive &t, float width, float height) {
  t.v[0].gl_Position.x = floor(t.v[0].gl_Position.x);
  t.v[0].gl_Position.y = floor(t.v[0].gl_Position.y);
  t.v[1].gl_Position.x = floor(t.v[1].gl_Position.x);
  t.v[1].gl_Position.y = floor(t.v[1].gl_Position.y);
  t.v[2].gl_Position.x = floor(t.v[2].gl_Position.x);
  t.v[2].gl_Position.y = floor(t.v[2].gl_Position.y);

  // Calculate bounding box
  efv.xMinMax[0] = glm::min(glm::min(t.v[0].gl_Position.x, t.v[1].gl_Position.x), t.v[2].gl_Position.x);
  efv.yMinMax[0] = glm::min(glm::min(t.v[0].gl_Position.y, t.v[1].gl_Position.y), t.v[2].gl_Position.y);
  efv.xMinMax[1] = glm::max(glm::max(t.v[0].gl_Position.x, t.v[1].gl_Position.x), t.v[2].gl_Position.x);
  efv.yMinMax[1] = glm::max(glm::max(t.v[0].gl_Position.y, t.v[1].gl_Position.y), t.v[2].gl_Position.y);

  // Check OOB
  efv.xMinMax[0] = glm::max(0.5f, efv.xMinMax[0]);
  efv.yMinMax[0] = glm::max(0.5f, efv.yMinMax[0]);
  efv.xMinMax[1] = glm::min(width, efv.xMinMax[1]);
  efv.yMinMax[1] = glm::min(height, efv.yMinMax[1]);

  efv.delta0[0] = t.v[1].gl_Position.x - t.v[0].gl_Position.x;
  efv.delta0[1] = t.v[1].gl_Position.y - t.v[0].gl_Position.y;

  efv.delta1[0] = t.v[2].gl_Position.x - t.v[1].gl_Position.x;
  efv.delta1[1] = t.v[2].gl_Position.y - t.v[1].gl_Position.y;

  efv.delta2[0] = t.v[0].gl_Position.x - t.v[2].gl_Position.x;
  efv.delta2[1] = t.v[0].gl_Position.y - t.v[2].gl_Position.y;


  efv.e[0] = (efv.yMinMax[0] - t.v[0].gl_Position.y) * efv.delta0[0] - (efv.xMinMax[0] - t.v[0].gl_Position.x) * efv.delta0[1];
  efv.e[1] = (efv.yMinMax[0] - t.v[1].gl_Position.y) * efv.delta1[0] - (efv.xMinMax[0] - t.v[1].gl_Position.x) * efv.delta1[1];
  efv.e[2] = (efv.yMinMax[0] - t.v[2].gl_Position.y) * efv.delta2[0] - (efv.xMinMax[0] - t.v[2].gl_Position.x) * efv.delta2[1];
}

/**
 * @brief Rasterize triangle
 * 
 */
void rasterizeWithPineda(Primitive &t, Frame &frame, Program &prg) {
  InFragment inFrag;
  OutFragment outFrag;
  EdgeFnValues efv;
  float lambda0_2d, lambda1_2d, lambda2_2d,
        lambda_s,
        lambda0, lambda1, lambda2,
        totalTriangleArea, invTotalTriangleArea,
        temp0, temp1, temp2;
  
  calculateEdgeFnValues(efv, t, (float)frame.width, (float)frame.height);

  // For barycentric coordinates
  totalTriangleArea = calculateTriangleArea(t.v[0].gl_Position, t.v[1].gl_Position, t.v[2].gl_Position);
  if(totalTriangleArea == 0.0f) { // No triangle to draw, discard
    return;
  }
  invTotalTriangleArea = 1.f / totalTriangleArea;

  for(float y = efv.yMinMax[0]; y < efv.yMinMax[1]; y++) {
    temp0 = efv.e[0];
    temp1 = efv.e[1];
    temp2 = efv.e[2];

    for(float x = efv.xMinMax[0]; x < efv.xMinMax[1]; x++) {
      if(temp0 >= 0.f && temp1 >= 0.f && temp2 >= 0.f) {
        inFrag.gl_FragCoord.x = x;
        inFrag.gl_FragCoord.y = y;

        // Barycentric coordinates
        lambda0_2d = calculateTriangleArea(t.v[1].gl_Position, t.v[2].gl_Position, inFrag.gl_FragCoord) * invTotalTriangleArea;
        lambda1_2d = calculateTriangleArea(t.v[2].gl_Position, t.v[0].gl_Position, inFrag.gl_FragCoord) * invTotalTriangleArea;
        lambda2_2d = 1.f - lambda0_2d - lambda1_2d;
        
        // Interpolate z
        inFrag.gl_FragCoord.z = 
                t.v[0].gl_Position.z * lambda0_2d
              + t.v[1].gl_Position.z * lambda1_2d
              + t.v[2].gl_Position.z * lambda2_2d;

        // Calculate the values to interpolate by - lambda
        lambda_s = 
                (lambda0_2d/t.v[0].gl_Position.w) 
              + (lambda1_2d/t.v[1].gl_Position.w)
              + (lambda2_2d/t.v[2].gl_Position.w);
        
        lambda0 = lambda0_2d / (t.v[0].gl_Position.w * lambda_s);
        lambda1 = lambda1_2d / (t.v[1].gl_Position.w * lambda_s);
        lambda2 = 1 - lambda0 - lambda1;

        // Interpolate attributes
        interpolateAttributes(inFrag, t, prg, lambda0, lambda1, lambda2);

        // Run the fragment shader
        prg.fragmentShader(outFrag, inFrag, prg.uniforms);

        uint64_t coordinates = (uint64_t) (((uint64_t)x) + (((uint64_t)y * frame.width)));

        // Depth test
        if(inFrag.gl_FragCoord.z < frame.depth[coordinates]) {
          float outA = outFrag.gl_FragColor.a;
          if(outA > 0.5f) {
            frame.depth[coordinates] = inFrag.gl_FragCoord.z;
          }

          if(outA == 1.0f) {
            frame.color[ coordinates * 4     ] = (uint8_t)(glm::clamp(outFrag.gl_FragColor.r, 0.f, 1.f) * 255.f); // R
            frame.color[(coordinates * 4) + 1] = (uint8_t)(glm::clamp(outFrag.gl_FragColor.g, 0.f, 1.f) * 255.f); // G
            frame.color[(coordinates * 4) + 2] = (uint8_t)(glm::clamp(outFrag.gl_FragColor.b, 0.f, 1.f) * 255.f); // B
          } else if(outA != 0.0f) {
            // Blend
            uint64_t  coordR =  coordinates * 4, 
                      coordG = (coordinates * 4) + 1,
                      coordB = (coordinates * 4) + 2,
                      coordA = (coordinates * 4) + 3;
            
            frame.color[coordR] = (uint8_t)(glm::clamp(
                  ((((float)frame.color[coordR]) / 255.f) * (1.f - outA) + outFrag.gl_FragColor.r * outA),
                  0.0f,
                  1.0f) * 255.f);
            frame.color[coordG] = (uint8_t)(glm::clamp(
                  ((((float)frame.color[coordG]) / 255.f) * (1.f - outA) + outFrag.gl_FragColor.g * outA),
                  0.0f,
                  1.0f) * 255.f);
            frame.color[coordB] = (uint8_t)(glm::clamp(
                  ((((float)frame.color[coordB]) / 255.f) * (1.f - outA) + outFrag.gl_FragColor.b * outA),
                  0.0f,
                  1.0f) * 255.f);
          } /* else discard fragment */
        }
      }

      temp0 -= efv.delta0[1];
      temp1 -= efv.delta1[1];
      temp2 -= efv.delta2[1];
    }
    efv.e[0] += efv.delta0[0];
    efv.e[1] += efv.delta1[0];
    efv.e[2] += efv.delta2[0];
  }
}

/**
 * @brief Calculate 't' value from: t = (-A.w - A.z) / (B.w - A.w + B.z - A.z)
 * 
 * @param A First point
 * @param B Second point
 * @return float t
 */
float getClipLineT(glm::vec4 &A, glm::vec4 &B) {
  float tDivisor = B.w - A.w + B.z - A.z;
  if(tDivisor == 0.f) {
    return 0.f;
  }

  return (-A.w - A.z) / (tDivisor);
}

/**
 * @brief Clip triangle with near plane
 * 
 * @param triangle Triangle to clip
 * @param clipped Output triangles:
 * clipped.t[] = Clipped triangles
 * clipped.pCount = How many triangles does clipped.t[] contains
 */
void clipping(Primitive &triangle, ClippedPrimitive &clipped, Program &prg) {
  float t[3] = { };
  OutVertex A, B, C;
  uint8_t clipCount = 0;

  // Check how many vertices to clip
  for(uint32_t i = 0; i < 3; i++) {
    if(-triangle.v[i].gl_Position.w > triangle.v[i].gl_Position.z) {
      clipCount++;
      t[i] = -1.0f;
    }
  }

  if(clipCount == 0) { // Dont clip
    clipped.pCount = 1;
    clipped.t[0] = triangle;
    return;
  }

  if(clipCount == 3) {
    A = triangle.v[0];
    B = triangle.v[1];
    C = triangle.v[2];

    float t1 = getClipLineT(B.gl_Position, A.gl_Position);
    float t2 = getClipLineT(C.gl_Position, B.gl_Position);
    float t3 = getClipLineT(A.gl_Position, C.gl_Position);

    /*if((t1 < 0.f || t1 > 1.f) && (t2 < 0.f || t2 > 1.f) && (t3 < 0.f || t3 > 1.f)) {
      clipped.pCount = 0; // clip all
      _LOG("CLIP ALL");
      return;
    }*/

    _LOG(t1 << " " << t2 << " " << t3);

    glm::vec4 Xt1 = B.gl_Position + t1 * (A.gl_Position - B.gl_Position);
    glm::vec4 Xt2 = C.gl_Position + t2 * (B.gl_Position - C.gl_Position);
    glm::vec4 Xt3 = A.gl_Position + t3 * (C.gl_Position - A.gl_Position);

    clipped.pCount = 1;

    clipped.t[0].v[0].gl_Position = Xt1;
    clipped.t[0].v[1].gl_Position = Xt2;
    clipped.t[0].v[2].gl_Position = Xt3;

    // Interpolate
    for(uint32_t i = 0; i < maxAttributes; i++) { // very ugly, but effective (i think)
      if(prg.vs2fs[i] == AttributeType::EMPTY) {
        clipped.t[0].v[0].attributes[i] = A.attributes[i];
        clipped.t[0].v[1].attributes[i] = B.attributes[i];
        clipped.t[0].v[2].attributes[i] = C.attributes[i];
        continue;
      }

      if(prg.vs2fs[i] == AttributeType::FLOAT) {
        clipped.t[0].v[0].attributes[i].v1 = B.attributes[i].v1 + t1 * (A.attributes[i].v1 - B.attributes[i].v1);
        clipped.t[0].v[1].attributes[i].v1 = C.attributes[i].v1 + t2 * (B.attributes[i].v1 - C.attributes[i].v1);
        clipped.t[0].v[2].attributes[i].v1 = A.attributes[i].v1 + t3 * (C.attributes[i].v1 - A.attributes[i].v1);
      } else if(prg.vs2fs[i] == AttributeType::VEC2) {
        clipped.t[0].v[0].attributes[i].v2 = B.attributes[i].v2 + t1 * (A.attributes[i].v2 - B.attributes[i].v2);
        clipped.t[0].v[1].attributes[i].v2 = C.attributes[i].v2 + t2 * (B.attributes[i].v2 - C.attributes[i].v2);
        clipped.t[0].v[2].attributes[i].v2 = A.attributes[i].v2 + t3 * (C.attributes[i].v2 - A.attributes[i].v2);
      } else if(prg.vs2fs[i] == AttributeType::VEC3) {
        clipped.t[0].v[0].attributes[i].v3 = B.attributes[i].v3 + t1 * (A.attributes[i].v3 - B.attributes[i].v3);
        clipped.t[0].v[1].attributes[i].v3 = C.attributes[i].v3 + t2 * (B.attributes[i].v3 - C.attributes[i].v3);
        clipped.t[0].v[2].attributes[i].v3 = A.attributes[i].v3 + t3 * (C.attributes[i].v3 - A.attributes[i].v3);
      } else {
        clipped.t[0].v[0].attributes[i].v4 = B.attributes[i].v4 + t1 * (A.attributes[i].v4 - B.attributes[i].v4);
        clipped.t[0].v[1].attributes[i].v4 = C.attributes[i].v4 + t2 * (B.attributes[i].v4 - C.attributes[i].v4);
        clipped.t[0].v[2].attributes[i].v4 = A.attributes[i].v4 + t3 * (C.attributes[i].v4 - A.attributes[i].v4);
      }
    }
    return;
  }

  // One vertex is oob, create 2 new triangles
  if(clipCount == 1) {
    // Find which one to clip
    if(t[0] == -1.f) {
      A = triangle.v[0];
      B = triangle.v[1];
      C = triangle.v[2];
    } else if(t[1] == -1.f) {
      A = triangle.v[1];
      B = triangle.v[2];
      C = triangle.v[0];
    } else {
      A = triangle.v[2];
      B = triangle.v[0];
      C = triangle.v[1];
    }

    float t1 = getClipLineT(B.gl_Position, A.gl_Position);
    float t2 = getClipLineT(C.gl_Position, A.gl_Position);

    glm::vec4 Xt1 = B.gl_Position + t1 * (A.gl_Position - B.gl_Position);
    glm::vec4 Xt2 = C.gl_Position + t2 * (A.gl_Position - C.gl_Position);

    // Create 2 new triangles
    clipped.pCount = 2;

    clipped.t[0].v[0].gl_Position = B.gl_Position;
    clipped.t[0].v[1].gl_Position = C.gl_Position;
    clipped.t[0].v[2].gl_Position = Xt1;

    clipped.t[1].v[0].gl_Position = Xt1;
    clipped.t[1].v[1].gl_Position = C.gl_Position;
    clipped.t[1].v[2].gl_Position = Xt2;

    // Interpolate
    for(uint32_t i = 0; i < maxAttributes; i++) { // very ugly, but effective (i think)
      clipped.t[0].v[0].attributes[i] = B.attributes[i];
      clipped.t[0].v[1].attributes[i] = C.attributes[i];

      clipped.t[1].v[1].attributes[i] = C.attributes[i];

      if(prg.vs2fs[i] == AttributeType::EMPTY) {
        clipped.t[0].v[2].attributes[i] = B.attributes[i];

        clipped.t[1].v[0].attributes[i] = A.attributes[i];
        clipped.t[1].v[2].attributes[i] = A.attributes[i];
        continue;
      }

      if(prg.vs2fs[i] == AttributeType::FLOAT) {
        clipped.t[0].v[2].attributes[i].v1 = B.attributes[i].v1 + t1 * (A.attributes[i].v1 - B.attributes[i].v1);
        clipped.t[1].v[0].attributes[i].v1 = clipped.t[0].v[2].attributes[i].v1;
        clipped.t[1].v[2].attributes[i].v1 = C.attributes[i].v1 + t2 * (A.attributes[i].v1 - C.attributes[i].v1);
      } else if(prg.vs2fs[i] == AttributeType::VEC2) {
        clipped.t[0].v[2].attributes[i].v2 = B.attributes[i].v2 + t1 * (A.attributes[i].v2 - B.attributes[i].v2);
        clipped.t[1].v[0].attributes[i].v2 = clipped.t[0].v[2].attributes[i].v2;
        clipped.t[1].v[2].attributes[i].v2 = C.attributes[i].v2 + t2 * (A.attributes[i].v2 - C.attributes[i].v2);
      } else if(prg.vs2fs[i] == AttributeType::VEC3) {
        clipped.t[0].v[2].attributes[i].v3 = B.attributes[i].v3 + t1 * (A.attributes[i].v3 - B.attributes[i].v3);
        clipped.t[1].v[0].attributes[i].v3 = clipped.t[0].v[2].attributes[i].v3;
        clipped.t[1].v[2].attributes[i].v3 = C.attributes[i].v3 + t2 * (A.attributes[i].v3 - C.attributes[i].v3);
      } else {
        clipped.t[0].v[2].attributes[i].v4 = B.attributes[i].v4 + t1 * (A.attributes[i].v4 - B.attributes[i].v4);
        clipped.t[1].v[0].attributes[i].v4 = clipped.t[0].v[2].attributes[i].v4;
        clipped.t[1].v[2].attributes[i].v4 = C.attributes[i].v4 + t2 * (A.attributes[i].v4 - C.attributes[i].v4);
      }
    }

    return;
  }
  // else clipCount == 2

  // C = not clipped
  if(t[0] == -1.f) {
    // 0 NOK
    if(t[1] == -1.f) { // 2 OK; 0,1 NOK
      A = triangle.v[0];
      B = triangle.v[1];
      C = triangle.v[2];
    } else { // 1 OK; 0,2 NOK
      A = triangle.v[2];
      B = triangle.v[0];
      C = triangle.v[1];
    }
  } else { // 0 OK; 1,2 NOK
    A = triangle.v[1];
    B = triangle.v[2];
    C = triangle.v[0];
  }

  float t1 = getClipLineT(C.gl_Position, A.gl_Position);
  float t2 = getClipLineT(C.gl_Position, B.gl_Position);

  glm::vec4 Xt1 = C.gl_Position + t1 * (A.gl_Position - C.gl_Position);
  glm::vec4 Xt2 = C.gl_Position + t2 * (B.gl_Position - C.gl_Position);

  clipped.pCount = 1;
  clipped.t[0].v[0].gl_Position = Xt1;
  clipped.t[0].v[1].gl_Position = Xt2;
  clipped.t[0].v[2].gl_Position = C.gl_Position;
  
  // Interpolate
  for(uint32_t i = 0; i < maxAttributes; i++) {
    clipped.t[0].v[2].attributes[i] = C.attributes[i];

    if(prg.vs2fs[i] == AttributeType::EMPTY) {
      clipped.t[0].v[0].attributes[i] = A.attributes[i];
      clipped.t[0].v[1].attributes[i] = B.attributes[i];
      continue;
    }

    if(prg.vs2fs[i] == AttributeType::FLOAT) {
      clipped.t[0].v[0].attributes[i].v1 = C.attributes[i].v1 + t1 * (A.attributes[i].v1 - C.attributes[i].v1);
      clipped.t[0].v[1].attributes[i].v1 = C.attributes[i].v1 + t2 * (B.attributes[i].v1 - C.attributes[i].v1);
    } else if(prg.vs2fs[i] == AttributeType::VEC2) {
      clipped.t[0].v[0].attributes[i].v2 = C.attributes[i].v2 + t1 * (A.attributes[i].v2 - C.attributes[i].v2);
      clipped.t[0].v[1].attributes[i].v2 = C.attributes[i].v2 + t2 * (B.attributes[i].v2 - C.attributes[i].v2);
    } else if(prg.vs2fs[i] == AttributeType::VEC3) {
      clipped.t[0].v[0].attributes[i].v3 = C.attributes[i].v3 + t1 * (A.attributes[i].v3 - C.attributes[i].v3);
      clipped.t[0].v[1].attributes[i].v3 = C.attributes[i].v3 + t2 * (B.attributes[i].v3 - C.attributes[i].v3);
    } else {
      clipped.t[0].v[1].attributes[i].v4 = C.attributes[i].v4 + t2 * (B.attributes[i].v4 - C.attributes[i].v4);
      clipped.t[0].v[0].attributes[i].v4 = C.attributes[i].v4 + t1 * (A.attributes[i].v4 - C.attributes[i].v4);
    }
  }
}

//! [drawImpl]
void drawImpl(GPUContext &ctx, uint32_t nofVertices) {
  Primitive t;
  uint8_t primitiveIndex = 0;

  for(uint32_t i = 0; i < nofVertices; i++) {
    InVertex inVertex;

    computeVertexID(inVertex, ctx.vao, i);
    readVertexAttributes(inVertex, ctx.vao);
    // Run the vertex shader
    ctx.prg.vertexShader(t.v[primitiveIndex], inVertex, ctx.prg.uniforms);

    // Construct a triangle ?
    if(primitiveIndex != 2) {
      primitiveIndex++;
      continue;
    } // else - triangle
    primitiveIndex = 0;

    // Determine winding order
    if(isClockwiseWindingOrder(t)) {
      if(ctx.backfaceCulling) {
        continue; // discard
      }
      // Swap vertex 1 with vertex 2 
      auto temp = t.v[1];
      t.v[1] = t.v[2];
      t.v[2] = temp;
    }

    /*** Clipping ***/
    ClippedPrimitive clipped {};
    clipping(t, clipped, ctx.prg);

    for(uint32_t j = 0; j < clipped.pCount; j++) {
      perspectiveDivisionAndVPTransformation(clipped.t[j], (float)ctx.frame.width, (float)ctx.frame.height);
      rasterizeWithPineda(clipped.t[j], ctx.frame, ctx.prg);
    }
  }
}
//! [drawImpl]

/**
 * @brief This function reads color from texture.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
glm::vec4 read_texture(Texture const&texture,glm::vec2 uv) {
  if(!texture.data)return glm::vec4(0.f);
  auto uv1 = glm::fract(uv);
  auto uv2 = uv1*glm::vec2(texture.width-1,texture.height-1)+0.5f;
  auto pix = glm::uvec2(uv2);
  //auto t   = glm::fract(uv2);
  glm::vec4 color = glm::vec4(0.f,0.f,0.f,1.f);
  for(uint32_t c=0;c<texture.channels;++c)
    color[c] = texture.data[(pix.y*texture.width+pix.x)*texture.channels+c]/255.f;
  return color;
}

/**
 * @brief This function clears framebuffer.
 *
 * @param ctx GPUContext
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void clear(GPUContext&ctx,float r,float g,float b,float a){
  auto&frame = ctx.frame;
  auto const nofPixels = frame.width * frame.height;
  for(size_t i=0;i<nofPixels;++i){
    frame.depth[i] = 10e10f;
    frame.color[i*4+0] = static_cast<uint8_t>(glm::min(r*255.f,255.f));
    frame.color[i*4+1] = static_cast<uint8_t>(glm::min(g*255.f,255.f));
    frame.color[i*4+2] = static_cast<uint8_t>(glm::min(b*255.f,255.f));
    frame.color[i*4+3] = static_cast<uint8_t>(glm::min(a*255.f,255.f));
  }
}

