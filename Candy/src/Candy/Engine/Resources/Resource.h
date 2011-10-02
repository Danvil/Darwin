#pragma once

namespace Candy
{

	class Ressource
	{
	public:
		Ressource()
		: _state(RessourceBare)
		{}

		virtual ~Ressource() {}

		virtual void Generate() = 0;

		virtual void Delete() = 0;

		void AssureGenerated() {
			if(_state == RessourceDeleted) {
				throw 0; // FIXME
			}
			if(_state == RessourceBare) {
				Generate();
				_state = RessourceGenerated;
			}
		}

	protected:
		enum State {
			RessourceBare,
			RessourceGenerated,
			RessourceDeleted
		};

		void set_state(State state) {
			_state = state;
		}

		State state() const {
			return _state;
		}

	private:
		State _state;
	};

}
