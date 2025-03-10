 #pragma once
#include <d3d11.h>
 #include <wrl.h>
 #include <directxmath.h>
 #include <vector>
 #include <string>
 #include <fbxsdk.h>
#include"texture.h"
#include"shader.h"
#include <filesystem>
#include <unordered_map>
#include <functional>


struct skeleton {
	struct bone {
		uint64_t unique_id{ 0 };
		std::string name;
		int64_t parent_index{ -1 }; 
		int64_t node_index{ 0 };

		
		DirectX::XMFLOAT4X4 offset_transform{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		bool is_orphan() const { return parent_index < 0; }
	};

	std::vector<bone> bones;
	int64_t indexof(uint64_t unique_id) const {
		int64_t index{ 0 };
		for (const bone& bone : bones) {
			if (bone.unique_id == unique_id) {
				return index;
			}
			++index;
		}
		return -1;
	}
};
 struct scene
 {
	 struct node
	 {
		 uint64_t unique_id{ 0 };
		 std::string name;
		 FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };
		 int64_t parent_index{ -1 };
		 };
	 std::vector<node> nodes;
	 int64_t indexof(uint64_t unique_id) const
		 {
		 int64_t index{ 0 };
		 for (const node& node : nodes)
			 {
			 if (node.unique_id == unique_id)
				 {
				 return index;
				 }
			  ++index;
		 }
		 return -1;
		 }
	 };

class skinned_mesh
 {
 public: 

	 static const int MAX_BONE_INFLUENCES{ 4 };
	 struct vertex
			 {
			 DirectX::XMFLOAT3 position;
			 DirectX::XMFLOAT3 normal{ 0, 1, 0 };
			 DirectX::XMFLOAT2 texcoord{ 0, 0 };
			 float bone_weights[MAX_BONE_INFLUENCES]{ 1, 0, 0, 0 };
			 uint32_t bone_indices[MAX_BONE_INFLUENCES]{};

			 };
		 struct constants
			 {
			 DirectX::XMFLOAT4X4 world;
			 DirectX::XMFLOAT4 material_color;
			 };
		 struct mesh
		 {

			 uint64_t unique_id{ 0 };
			 std::string name;
			 int64_t node_index{ 0 };
			 std::vector<vertex> vertices;
			 std::vector<uint32_t> indices;
			 struct subset
			 {
				 uint64_t material_unique_id{ 0 };
				 std::string material_name;
				 uint32_t start_index_location{ 0 };
				 uint32_t index_count{ 0 };
			 };
			 DirectX::XMFLOAT4X4 default_global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			 std::vector<subset> subsets;
		 private:
			 Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
			 Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
			
			 friend class skinned_mesh;

		 };
		 struct material
		 {
			 uint64_t unique_id{ 0 };
			 std::string name;

			 DirectX::XMFLOAT4 Ka{ 0.2f, 0.2f, 0.2f, 1.0f };
			 DirectX::XMFLOAT4 Kd{ 0.8f, 0.8f, 0.8f, 1.0f };
			 DirectX::XMFLOAT4 Ks{ 1.0f, 1.0f, 1.0f, 1.0f };

			 std::string texture_filenames[4];
			 Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[4];
		 };
		 std::unordered_map<uint64_t,material> materials;
		 private:
			 Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
			 Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
			 Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
			 Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
			public:
				std::vector<mesh> meshes;
				
				

				 skinned_mesh(ID3D11Device * device, const char* fbx_filename, bool triangulate = false);
				 virtual ~skinned_mesh() = default;
				 void skinned_mesh::fetch_materials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials);
				 void skinned_mesh::create_com_objects(ID3D11Device* device, const char* fbx_filename);	
				 void skinned_mesh::fetch_meshes(FbxScene* fbx_scene, std::vector<mesh>& meshes);
				 void skinned_mesh::render(ID3D11DeviceContext* immediate_context,
					 const XMFLOAT4X4& world, const XMFLOAT4& material_color);
				 void skinned_mesh::fetch_skeleton(FbxMesh* fbx_mesh, skeleton& bind_pose, const scene& scene_view);
			
				 inline XMFLOAT4X4 to_xmfloat4x4(const FbxAMatrix& fbxamatrix)
				 {
					 XMFLOAT4X4 xmfloat4x4;
					 for (int row = 0; row < 4; ++row)
					 {
						 for (int column = 0; column < 4; ++column)
						 {
							 xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
						 }
					 }
					 return xmfloat4x4;
				 }
				 inline XMFLOAT3 to_xmfloat3(const FbxDouble3& fbxdouble3)
				 {
					 XMFLOAT3 xmfloat3;
					 xmfloat3.x = static_cast<float>(fbxdouble3[0]);
					 xmfloat3.y = static_cast<float>(fbxdouble3[1]);
					 xmfloat3.z = static_cast<float>(fbxdouble3[2]);
					 return xmfloat3;
				 }
				 inline XMFLOAT4 to_xmfloat4(const FbxDouble4& fbxdouble4)
				 {
					 XMFLOAT4 xmfloat4;
					 xmfloat4.x = static_cast<float>(fbxdouble4[0]);
					 xmfloat4.y = static_cast<float>(fbxdouble4[1]);
					 xmfloat4.z = static_cast<float>(fbxdouble4[2]);
					 xmfloat4.w = static_cast<float>(fbxdouble4[3]);
					 return xmfloat4;
				 }
				 protected:
					 scene scene_view;
					
					 };
