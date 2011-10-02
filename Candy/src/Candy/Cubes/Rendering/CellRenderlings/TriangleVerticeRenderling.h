#pragma once

#include "../../Cubes.h"
#include "../CellRenderlingInterface.h"
#include <GL/glew.h>
#include <Candy/Engine/Mesh.h>
#include <Candy/Engine/Uniform.h>
#include <boost/shared_array.hpp>
#include <boost/thread.hpp>
#include <vector>
#include <iostream>

template<typename MeshTraits>
class TriangleVerticeRenderling
: public CellRenderlingInterface
{
public:
	TriangleVerticeRenderling(Cubes* cubes, Cell* cell)
	: _cubes(cubes), _cell(cell) {
		_is_dirty = true;
		_needs_transfer = false;
	}

	virtual ~TriangleVerticeRenderling() {}

protected:
	virtual void PrepareUpdateMesh() {}

	virtual void FinishUpdateMesh() {}

	virtual void PrepareRender() {}

	virtual void FinishRender() {}

	Cell* GetCell() const {
		return _cell;
	}

public:
	void Render() {
		//std::cout << "Rendering cell " << _cell->coordinate() << std::endl;

		TransferMesh();

		if(_cell->HasBorder()) {
			PrepareRender();
			_mesh.Render();
			FinishRender();
		}
	}

	void Invalidate() {
		_is_dirty = true;
	}

	void UpdateMesh() {
		if(_is_dirty) {
			// do not compute mesh for dirty meshes
			if(_cell->NeedsVitalization()) {
				return;
			}
			_is_dirty = false;
			if(_cell->HasBorder()) {
				_mutex.lock();
				PrepareUpdateMesh();
				_cubes->CreateMeshData<typename MeshTraits::Vertex, typename MeshTraits::AddSideVertex, Candy::MeshData<typename MeshTraits::Vertex> >(_cell, _mesh_data);
				FinishUpdateMesh();
				_mutex.unlock();
				//std::cout << "Recreated mesh: index_count=" << _mesh_data.indices()->size() << ", vertex_count=" << _mesh_data.vertices()->size() << std::endl;
			} else {
				_mesh_data.indices()->clear();
				_mesh_data.vertices()->clear();
			}
			_needs_transfer = true;
		}
	}

	void TransferMesh() {
		if(_needs_transfer) {
			_needs_transfer = false;
			_mutex.lock();
			_mesh.UpdateMesh(_mesh_data);
			_mutex.unlock();
		}
	}

private:
	Cubes* _cubes;

	Cell* _cell;

	bool _is_dirty;

	bool _needs_transfer;

	typename MeshTraits::Mesh _mesh;

	Candy::MeshData<typename MeshTraits::Vertex> _mesh_data;

	boost::mutex _mutex;

protected:
	//boost::thread mesh_computation_thread;

};
