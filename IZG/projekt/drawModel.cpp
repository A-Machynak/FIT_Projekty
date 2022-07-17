/*!
 * @file
 * @brief This file contains functions for model rendering
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#include <student/drawModel.hpp>
#include <student/gpu.hpp>

#define POS_PROJVIEW_mat 0
#define POS_MODEL_mat 1
#define POS_INVTRANS_mat 2
#define POS_LIGHT_v3 3
#define POS_CAMERA_v3 4
#define POS_DIFFUSE_COLOR_v4 5
#define POS_DIFFUSE_INDICATOR_f 6
#define POS_DOUBLESIDED_f 7

void drawNode(GPUContext &ctx, Node const &node, Model const &model, const glm::mat4 &mat) {
  auto newMatrix = mat * node.modelMatrix;

  if(node.mesh >= 0) {
    const Mesh *mesh = &model.meshes[node.mesh];

    // Uniforms
    ctx.prg.uniforms.uniform[POS_MODEL_mat].m4 = newMatrix;
    ctx.prg.uniforms.uniform[POS_INVTRANS_mat].m4 = glm::transpose(glm::inverse(newMatrix));
    ctx.prg.uniforms.uniform[POS_DIFFUSE_COLOR_v4].v4 = mesh->diffuseColor;
    if(mesh->diffuseTexture >= 0) {
      ctx.prg.uniforms.textures[0] = model.textures[mesh->diffuseTexture];
      ctx.prg.uniforms.uniform[POS_DIFFUSE_INDICATOR_f].v1 = 1.f;
    } else {
      ctx.prg.uniforms.textures[0] = Texture();
      ctx.prg.uniforms.uniform[POS_DIFFUSE_INDICATOR_f].v1 = 0.f;
    }

    // Attributes
    ctx.vao.vertexAttrib[0] = mesh->position;
    ctx.prg.vs2fs[0] = AttributeType::VEC3;
    ctx.vao.vertexAttrib[1] = mesh->normal;
    ctx.prg.vs2fs[1] = AttributeType::VEC3;
    ctx.vao.vertexAttrib[2] = mesh->texCoord;
    ctx.prg.vs2fs[2] = AttributeType::VEC2;

    // Index buffer
    ctx.vao.indexType = mesh->indexType;
    ctx.vao.indexBuffer = mesh->indices;
    if(mesh->doubleSided) {
      ctx.prg.uniforms.uniform[POS_DOUBLESIDED_f].v1 = 1.f;
    } else {
      ctx.prg.uniforms.uniform[POS_DOUBLESIDED_f].v1 = 0.f;
    }
    ctx.backfaceCulling = mesh->doubleSided;

    draw(ctx, mesh->nofIndices);
  }

  for(size_t i = 0; i < node.children.size(); i++) {
    drawNode(ctx, node.children[i], model, newMatrix);
  }
}

/**
 * @brief This function renders a model
 *
 * @param ctx GPUContext
 * @param model model structure
 * @param proj projection matrix
 * @param view view matrix
 * @param light light position
 * @param camera camera position (unused)
 */
//! [drawModel]
void drawModel(GPUContext&ctx,Model const&model,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
  ctx.prg.vertexShader = drawModel_vertexShader;
  ctx.prg.fragmentShader = drawModel_fragmentShader;

  ctx.prg.uniforms.uniform[POS_PROJVIEW_mat].m4 = proj * view;
  ctx.prg.uniforms.uniform[POS_LIGHT_v3].v3 = light;
  ctx.prg.uniforms.uniform[POS_CAMERA_v3].v3 = camera;

  glm::mat4 identityMat = glm::mat4(1.f);
  for(size_t i = 0; i < model.roots.size(); i++) {
    drawNode(ctx, model.roots[i], model, identityMat);
  }
}
//! [drawModel]

/**
 * @brief This function represents vertex shader of texture rendering method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
//! [drawModel_vs]
void drawModel_vertexShader(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
  auto const position = glm::vec4(inVertex.attributes[0].v3, 1.f);
  auto const normal = glm::vec4(inVertex.attributes[1].v3, 0.f);
  auto const& texUV = inVertex.attributes[2].v2;

  auto const& projViewMatrix = uniforms.uniform[POS_PROJVIEW_mat].m4;
  auto const& modelMatrix = uniforms.uniform[POS_MODEL_mat].m4;
  auto const& invTranspModelMatrix = uniforms.uniform[POS_INVTRANS_mat].m4;

  auto worldModelMat = modelMatrix * position;
 
  outVertex.gl_Position = projViewMatrix * worldModelMat;

  outVertex.attributes[0].v3 = worldModelMat;
  outVertex.attributes[1].v3 = invTranspModelMatrix * normal;
  outVertex.attributes[2].v2 = texUV;
}
//! [drawModel_vs]

/**
 * @brief This functionrepresents fragment shader of texture rendering method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
//! [drawModel_fs]
void drawModel_fragmentShader(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
  static const float aF = 0.2f; // Ambient factor
  float dF; // Diffuse factor
  glm::vec4 dC; // Diffuse color
  glm::vec4 aL; // Ambient
  glm::vec4 dL; // Diffuse

  auto const& lightPos = uniforms.uniform[POS_LIGHT_v3].v3;
  auto const& cameraPos = uniforms.uniform[POS_CAMERA_v3].v3;
  auto const& diffuseTxtBool = uniforms.uniform[POS_DIFFUSE_INDICATOR_f].v1;
  auto const& dblSided = uniforms.uniform[POS_DOUBLESIDED_f].v1;

  auto const& inPos = inFragment.attributes[0].v3;
  auto const& inNormal = inFragment.attributes[1].v3;
  auto const& inTexture = inFragment.attributes[2].v2;

  // Normalize the normal
  auto N = glm::normalize(inNormal);
  auto L = glm::normalize(lightPos - inPos);

  if(dblSided > 0.f) {
    if(glm::dot(N, cameraPos) < 0.f) {
      N = -N;
    }
  }

  if(diffuseTxtBool > 0.f) { // Texture is available
    dC = read_texture(uniforms.textures[0], inTexture);
  } else {
    dC = uniforms.uniform[POS_DIFFUSE_COLOR_v4].v4;
  }
  // Calculate diffuse factor
  dF = glm::clamp(glm::dot(L, N), 0.f, 1.f);

  aL = dC * aF;
  dL = dC * dF;

  glm::vec3 outColor = aL + dL;
  
  outFragment.gl_FragColor = glm::vec4(outColor, dC.a);
}
//! [drawModel_fs]

