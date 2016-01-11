#ifndef FUZZY_MODEL_TYPES_BASE
#define FUZZY_MODEL_TYPES_BASE

namespace model {
	namespace types {
		class Base {
		private:
			unsigned char _confidence;

		public:

			Base(unsigned char confidence) {
				if (confidence > 100) confidence = 100;
				_confidence = confidence;
			}

			unsigned char confidence() const {
				return _confidence;
			}

			void confidence(unsigned char confidence) {
				_confidence = confidence;
			}
		};

		class ConfidenceCompare {
		public:
			bool operator() (Base a, Base b)
			{
				return a.confidence() > b.confidence();
			}
		};
	}
}


#endif // !FUZZY_MODEL_TYPES_BASE
