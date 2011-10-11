/*
 * Pipe.hpp
 *
 *  Created on: Oct 11, 2011
 *      Author: david
 */

#ifndef PIPE_HPP_
#define PIPE_HPP_

#include "../StaticEntity.hpp"

namespace Tangerin { namespace Static {

class Pipe
: public StaticEntity
{
public:
	Pipe();

	virtual ~Pipe();

	void Tick(float dt, float total);

};

}}

#endif
