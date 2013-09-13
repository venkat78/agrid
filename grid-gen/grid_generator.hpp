#ifndef _GRID_GEN_GRID_GENERATOR_HPP_
#define _GRID_GEN_GRID_GENERATOR_HPP_

#include "defs.hpp"
#include "model_defn_to_grid_defn.hpp"
#include "manifold_obj.hpp"

namespace grid_gen {

  template<typename _MODEL_TYPE>
  class tGRID_GENERATOR {
      typedef typename tDEFINE_MODEL_GRID<_MODEL_TYPE>::cGRID cGRID;
      typedef typename tDEFINE_MODEL_GRID<_MODEL_TYPE>::cGRID_CELL cGRID_CELL;
      typedef typename _MODEL_TYPE::cMESH cMESH;
      typedef typename cMESH::cMANIFOLD cMANIFOLD;
      typedef tMANIFOLD_OBJ<cGRID, cMESH> cMANIFOLD_OBJ;

     public:
      tGRID_GENERATOR() {
        m_numCells[0] = m_numCells[1] = m_numCells[2] = 0;
      }

      tGRID_GENERATOR(const cBOX3 &simVol, INT numCells[3])
          : m_grid(simVol, numCells),
            m_bounds(simVol) {
        m_numCells[0] = numCells[0];
        m_numCells[1] = numCells[1];
        m_numCells[2] = numCells[2];
      }

      /*
       *  Registers model in the grid generator.
       */
      VOID Register(_MODEL_TYPE *model);
      VOID Build(cVOLUMETRIC_GRID *grid);

     private:

      /*
       *  1. Registers one manifold at a time.
       *  2. Clips each facet to coarse cell size.
       */
      VOID Register(cMESH *mesh);

      /*
       * Slices the facets of a mesh, so that they are confined
       * to coarse cells of m_grid.
       */
      VOID Slice(cMESH *mesh);

      /*
       * Register facets of the manifold in the coarse level
       * grid.
       */
      VOID RegisterManifold(cMANIFOLD *manifold, cMESH *mesh);

      /*
       * Registers facets in m_grid and manifold grid also.
       */
      VOID RegisterFacetsInGrids(cMESH *mesh, INT offset);

      VOID Populate(std::vector<cGRID_CELL*> &cells);
      BOOL Process(cGRID_CELL *cell);
      BOOL StitchCutCells(std::vector<cGRID_CELL*> &voxels, cVOLUMETRIC_GRID *grid);
     private:
      /*
       * manifolds recorded in the grid.
       */
      tTABLE<cMANIFOLD_OBJ> m_manifolds;
      cGRID m_grid;
      cBOX3 m_bounds;
      INT m_numCells[3];
  };

}

#endif //_GRID_GEN_GRID_GENERATOR_HPP_
