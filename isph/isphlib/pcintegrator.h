#ifndef ISPH_PCINTEGRATOR_H
#define ISPH_PCINTEGRATOR_H

#include "abstractintegrator.h"

namespace isph {

	/*!
	 *	\class	PCIntegrator
	 *	\brief	Modified Euler or Predictor Corrector Integrator.
	 */
	template<int dim, typename typ>
	class PCIntegrator: public AbstractIntegrator<dim,typ>
	{
	public:

		PCIntegrator(Simulation<dim,typ>* simulation);

		PCIntegrator(Simulation<dim,typ>* simulation, bool regrid);

		virtual ~PCIntegrator();

		bool Integrate();
    
	};

} // namespace isph

#include "pcintegrator.h"

#endif
