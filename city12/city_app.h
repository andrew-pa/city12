#pragma once
#include <dxut/dxut.h>
#include "meshgen.h"

struct city_app : public DXWindow, public DXDevice {

	city_app()
		: DXWindow(3100, 2000, L"City 12"), DXDevice() {}


	StepTimer tim;

	SimpleCamera cam;

	descriptor_heap rtv_heap;
	#define num_bufs 3
	ComPtr<ID3D12Resource> bufs[num_bufs];
	descriptor_heap rtsrv_heap;

	mesh cube_mesh;
	mesh beacon_mesh;
	mesh sidewalk_mesh;
	ComPtr<ID3D12Resource> inst_res, beacon_inst_res, sidewalk_inst_res;
	D3D12_VERTEX_BUFFER_VIEW inst_vbv, beacon_inst_vbv, sidewalk_inst_vbv;
	pass building_pass, beacon_pass, object_pass, ground_pass, sidewalk_pass, blur_pass, postprocess_pass;
	uint32_t num_inst, num_beacons;

	mesh fsq_mesh;

	const float block_world_size = 50.f;
	const float grid_world_size = block_world_size * 100.f;
	const float street_width = 15.f;
	const float ac_block_world_size = block_world_size - street_width;
	const float num_blocks = grid_world_size / block_world_size;
	const float min_height = 15.f;
	const float max_height = 60.f;
	const float beacon_min_height = 90.f;

	inline float calc_rand_height() {
		return powf(randf(),3.f)*(max_height - min_height) + min_height +
			(randf() < 0.02f ? 70.f*(randf()+0.3f) : 0.f);
	}

	void create_building(float xPos, float zPos, float w, float l, vector<XMFLOAT4X4>& d, vector<XMFLOAT4X4>& beaconT) {
		auto i = d.size();
		d.push_back(XMFLOAT4X4());
		float h = calc_rand_height();
		XMStoreFloat4x4(&d[i],
			XMMatrixScaling(w, h, l) *
			XMMatrixTranslation(xPos + w*.5f, h*.5f, zPos + l*.5f));
		if (h > beacon_min_height) {
			auto j = beaconT.size();
			beaconT.push_back(XMFLOAT4X4());
			XMStoreFloat4x4(&beaconT[j], XMMatrixTranslation(xPos + w*.5f, h + 2.f, zPos + l*.5f));
		}
	}

	void OnInit() override {
		init_d3d(this, 1, false);

		commandList = create_command_list(D3D12_COMMAND_LIST_TYPE_DIRECT);

#pragma region Create City
		cube_mesh = mesh(this, commandList, generate_cube_mesh(XMFLOAT3(1.f,1.f,1.f)));
		beacon_mesh = mesh(this, commandList, generate_sphere_mesh(.7f, 16, 16));
		/*
			0--8----9--1
			|  l    l  |
			|  4----5  |
			|  |    |  |
			|  7----6  |
			|  l    l  |
			3--E----X--2
			   ^~~d~^~r^
			d = 0.5, r = 0.25
		*/
		const float sidewalk_width = 0.05f; //percentage
		sidewalk_mesh = mesh(this, commandList, meshgen::qmesh({
			vertex(0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f), //0
			vertex(0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f), //1
			vertex(1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f), //2
			vertex(1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f), //3
			
			vertex(sidewalk_width, 0.f, sidewalk_width, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f), //4
			vertex(sidewalk_width, 0.f, 1.f-sidewalk_width, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f), //5
			vertex(1.f-sidewalk_width, 0.f, 1.f-sidewalk_width, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f), //6
			vertex(1.f-sidewalk_width, 0.f, sidewalk_width, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f), //7

			vertex(0.f, 0.f, sidewalk_width, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f), //8
			vertex(0.f, 0.f, 1.f-sidewalk_width, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f), //9
			vertex(1.f, 0.f, 1.f-sidewalk_width, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f), //X
			vertex(1.f, 0.f, sidewalk_width, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f), //E


			vertex(0.f, -0.4f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f), //12 -> 0
			vertex(0.f, -0.4f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f), //13 -> 1
			vertex(1.f, -0.4f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f), //14 -> 2
			vertex(1.f, -0.4f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f), //15 -> 3
		}, {
			{0, 8, 11, 3},
			{9, 1, 2, 10},
			{8, 9, 5, 4},
			{7, 6, 10, 11},
			{0,3,15,12},
			{2,1,13,14},
			{1,0,12,13},
			{3,2,14,15}
		}).generate());

		vector<XMFLOAT4X4> d; vector<XMFLOAT4X4> beaconT; vector<XMFLOAT4X4> swT;
		XMMATRIX sidewalkAllT = XMMatrixScaling(ac_block_world_size + ac_block_world_size*sidewalk_width*2.2f, 1.f, ac_block_world_size + ac_block_world_size*sidewalk_width*2.2f);

		const float size_of_alley = 0.1f; //of ac_block_world_size
		for (int z = 0; z < num_blocks; ++z) {
			float zPos = (float)z * block_world_size + (-grid_world_size*.5f) + street_width;
			for (int x = 0; x < num_blocks; ++x) {
				float xPos = (float)x * block_world_size + (-grid_world_size*.5f) + street_width;

				float	pW = randf()*0.2f+0.4f, 
						pL = randf()*0.2f+0.4f;
				create_building(xPos, zPos, ac_block_world_size*(pW - size_of_alley), ac_block_world_size*(pL - size_of_alley), d, beaconT);
				create_building(xPos + ac_block_world_size*pW, zPos, ac_block_world_size*(1.f - pW), ac_block_world_size*(pL - size_of_alley), d, beaconT);
				create_building(xPos, zPos + ac_block_world_size*pL, ac_block_world_size*(pW - size_of_alley), ac_block_world_size*(1.f - pL), d, beaconT);
				create_building(xPos + ac_block_world_size*pW, zPos + ac_block_world_size*pL, ac_block_world_size*(1.f - pW), ac_block_world_size*(1.f - pL), d, beaconT);

				auto swi = swT.size();
				swT.push_back(XMFLOAT4X4());
				XMStoreFloat4x4(&swT[swi], sidewalkAllT * XMMatrixTranslation(xPos - ac_block_world_size*sidewalk_width*1.1f, .5f, zPos - ac_block_world_size*sidewalk_width*1.1f));
			}
		}

		mesh::create_instance_buffer(this, commandList, d.data(), d.size()*sizeof(XMFLOAT4X4), sizeof(XMFLOAT4X4), &inst_vbv, inst_res);
		mesh::create_instance_buffer(this, commandList, beaconT.data(), beaconT.size() * sizeof(XMFLOAT4X4), sizeof(XMFLOAT4X4), &beacon_inst_vbv, beacon_inst_res);
		mesh::create_instance_buffer(this, commandList, swT.data(), swT.size() * sizeof(XMFLOAT4X4), sizeof(XMFLOAT4X4), &sidewalk_inst_vbv, sidewalk_inst_res);
		num_inst = d.size();
		num_beacons = beaconT.size();
#pragma endregion

		fsq_mesh = mesh(this, commandList, generate_quad_mesh(XMFLOAT2(1.f,1.f), false));
		
		commandList->Close();

		execute_command_list();

		rtv_heap = descriptor_heap(device, num_bufs, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false);
		rtsrv_heap = descriptor_heap(device, num_bufs, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);
		
		for (int i = 0; i < num_bufs; ++i) {
			chk(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, 
				&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, color_black), IID_PPV_ARGS(&bufs[i])));
			device->CreateRenderTargetView(bufs[i].Get(), nullptr, rtv_heap.cpu_handle(i));
			device->CreateShaderResourceView(bufs[i].Get(), nullptr, rtsrv_heap.cpu_handle(i));
			bufs[i]->SetName(L"Intermediate Buffer");
		}

		const D3D12_INPUT_ELEMENT_DESC input_layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

		};
		const D3D12_INPUT_ELEMENT_DESC inst_input_layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			
			{ "TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
			{ "TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
			{ "TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
			{ "TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },

		};
		
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pdesc = {};
		pdesc.InputLayout = { inst_input_layout, _countof(inst_input_layout) };
		pdesc.VS = load_shader(GetAssetFullPath(L"instance.vs.cso"));
		pdesc.PS = load_shader(GetAssetFullPath(L"basic.ps.cso"));
		pdesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		pdesc.RasterizerState.FrontCounterClockwise = true;
		//pdesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		pdesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pdesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		pdesc.SampleMask = UINT_MAX;
		pdesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pdesc.NumRenderTargets = 2;
		pdesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pdesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pdesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pdesc.SampleDesc.Count = 1;
		building_pass = pass(this, {
			root_parameterh::constants(16, 0),
		}, {}, pdesc, L"Building");

		pdesc.PS = load_shader(GetAssetFullPath(L"sidewalk.ps.cso"));
		sidewalk_pass = pass(this, building_pass.root_sig, pdesc);

		pdesc.PS = load_shader(GetAssetFullPath(L"beacon.ps.cso"));
		beacon_pass = pass(this, {
			root_parameterh::constants(16, 0),
			root_parameterh::constants(1, 1)
		}, {}, pdesc, L"Beacon");

		pdesc.VS = load_shader(GetAssetFullPath(L"basic.vs.cso"));

		pdesc.InputLayout = { input_layout, _countof(input_layout) };
		pdesc.PS = load_shader(GetAssetFullPath(L"ground.ps.cso"));
		object_pass = pass(this, {
			root_parameterh::constants(16, 0),
			root_parameterh::constants(16, 1),
		}, {}, pdesc, L"Object");
		ground_pass = pass(this, object_pass.root_sig, pdesc);

		pdesc = {};
		pdesc.InputLayout = { input_layout, _countof(input_layout) };
		pdesc.VS = load_shader(GetAssetFullPath(L"fsq.vs.cso"));
		pdesc.PS = load_shader(GetAssetFullPath(L"blur.ps.cso"));
		pdesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		//pdesc.RasterizerState.FrontCounterClockwise = true;
		pdesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pdesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		pdesc.DepthStencilState.DepthEnable = false;
		pdesc.SampleMask = UINT_MAX;
		pdesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pdesc.NumRenderTargets = 1;
		pdesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pdesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
		pdesc.SampleDesc.Count = 1;
		blur_pass = pass(this, {
			root_parameterh::constants(3, 0),
			root_parameterh::descriptor_table(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0)
		}, {
			CD3DX12_STATIC_SAMPLER_DESC(0)
		}, pdesc, L"Blur");

		pdesc.PS = load_shader(GetAssetFullPath(L"postprocess.ps.cso"));
		postprocess_pass = pass(this, {
			root_parameterh::constants(2, 0),
			root_parameterh::descriptor_table(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0),
			root_parameterh::descriptor_table(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1)
		}, {
			CD3DX12_STATIC_SAMPLER_DESC(0) 
		}, pdesc, L"Postprocess");


		wait_for_gpu();
		empty_upload_pool();

		cam.Init(XMFLOAT3(0.f, 35.f, 5.f));
		cam.moveSpeed = 100.f;
	}

	void OnUpdate() override {
		tim.Tick(nullptr);
		cam.Update(tim.GetElapsedSeconds());
	}

	void OnRender() override {
		start_frame();

		float t = tim.GetTotalSeconds();


		XMFLOAT4X4 camT; XMStoreFloat4x4(&camT,
			cam.GetViewMatrix()*cam.GetProjectionMatrix(45.f, aspectRatio, 1.f, 1000.f));

		//wchar_t out[32];
		//wsprintf(out, L"FPS: %d\n", tim.GetFramesPerSecond());
		//OutputDebugStringW(out);
		chk(commandAllocator->Reset());
		chk(commandList->Reset(commandAllocator.Get(), nullptr));

		auto cl = commandList;
		
		//render scene -> (color buffer, extra brightness buffer)
		set_default_viewport(cl);
		resource_barrier(cl, {
			CD3DX12_RESOURCE_BARRIER::Transition(bufs[0].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
			CD3DX12_RESOURCE_BARRIER::Transition(bufs[1].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
			CD3DX12_RESOURCE_BARRIER::Transition(bufs[2].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET) //won't need this quite yet but might as well
		});
		cl->OMSetRenderTargets(2, &rtv_heap.cpu_handle(), true, &dsvHeap->cpu_handle());
		cl->ClearRenderTargetView(rtv_heap.cpu_handle(0), color_black, 0, nullptr);
		cl->ClearRenderTargetView(rtv_heap.cpu_handle(1), color_black, 0, nullptr);
		cl->ClearDepthStencilView(dsvHeap->cpu_handle(), D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);

		building_pass.apply(cl);
		cl->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cl->SetGraphicsRoot32BitConstants(0, 16, &camT, 0);
		cube_mesh.draw(cl, num_inst, { inst_vbv });

		beacon_pass.apply(cl);
		cl->SetGraphicsRoot32BitConstants(0, 16, &camT, 0);
		cl->SetGraphicsRoot32BitConstants(1, 1, &t, 0);
		beacon_mesh.draw(cl, num_beacons, { beacon_inst_vbv });

		ground_pass.apply(cl);
		cl->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cl->SetGraphicsRoot32BitConstants(0, 16, &camT, 0);
		XMFLOAT4X4 T; XMStoreFloat4x4(&T,
			XMMatrixScaling(grid_world_size, .1f, grid_world_size));
		cl->SetGraphicsRoot32BitConstants(1, 16, &T, 0);
		cube_mesh.draw(cl);

		sidewalk_pass.apply(cl);
		cl->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cl->SetGraphicsRoot32BitConstants(0, 16, &camT, 0);
		sidewalk_mesh.draw(cl, num_blocks*num_blocks, { sidewalk_inst_vbv });
		

		resource_barrier(cl, {
			CD3DX12_RESOURCE_BARRIER::Transition(bufs[0].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
			CD3DX12_RESOURCE_BARRIER::Transition(bufs[1].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
		});

		float res[] = { width,height };
		//blur extra brightness buffer using bufs[1]&bufs[2]
		int src_buf = 1;
		int dst_buf = 2;
		cl->SetDescriptorHeaps(1, &rtsrv_heap.heap);
		for (int i = 0; i < 20; ++i) {
			cl->OMSetRenderTargets(1, &rtv_heap.cpu_handle(dst_buf), false, nullptr);
			
			blur_pass.apply(cl);
			cl->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			cl->SetGraphicsRoot32BitConstants(0, 2, &res, 0);
			cl->SetGraphicsRoot32BitConstant(0, i % 2, 2);
			cl->SetGraphicsRootDescriptorTable(1, rtsrv_heap.gpu_handle(src_buf));
			fsq_mesh.draw(cl);

			resource_barrier(cl, {
				CD3DX12_RESOURCE_BARRIER::Transition(bufs[src_buf].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
				CD3DX12_RESOURCE_BARRIER::Transition(bufs[dst_buf].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
			});
			swap(src_buf, dst_buf);
		}
		
		//postprocess step, (color, extra) -> backbuffer
		start_render_to_backbuffer(cl, true, false);
		cl->SetDescriptorHeaps(1, &rtsrv_heap.heap);
		postprocess_pass.apply(cl);
		cl->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cl->SetGraphicsRoot32BitConstants(0, 2, &res, 0);
		cl->SetGraphicsRootDescriptorTable(1, rtsrv_heap.gpu_handle(0));
		cl->SetGraphicsRootDescriptorTable(2, rtsrv_heap.gpu_handle(dst_buf));
		fsq_mesh.draw(cl);

		finish_render_to_backbuffer(cl);

		
		chk(cl->Close());

		execute_command_list();
		signal_queue();
		wait_for_gpu();

		next_frame();
	}

	void OnDestroy() override {
		wait_for_gpu();
		destroy_d3d();
	}

	bool OnEvent(MSG msg) override {
		switch (msg.message)
		{
		case WM_KEYDOWN:
			cam.OnKeyDown(msg.wParam);
			return true;

		case WM_KEYUP:
			cam.OnKeyUp(msg.wParam);
			return true;
		}
		return false;
	}
};
