#ifndef KT_APP_SERVICE_H_
#define KT_APP_SERVICE_H_

#include <string>

namespace kt {

/**
 * @class kt::Service
 * @brief Generic pluggable component.
 * NOTE: All subclasses need to implement a name function:
 * static const std::string& NAME() { static const std::string N("name"); return N; }
 * The name must be system-wide unique.
 */
class Service {
public:
	// Sunbclasses must implement this (replacing "name" with something system-wide unique)
	// static const std::string& NAME() { static const std::string N("name"); return N; }
	virtual ~Service()				{ }

	virtual void					shutdown() { }

protected:
	Service()						{ }
};

} // namespace kt

#endif
