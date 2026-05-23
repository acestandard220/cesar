#pragma once
#include "../cesar_core.h"
#include <random>

#define CESAR_GEN_UUID cesar::UUIDFactory::generateUUID()
#define CESAR_INVALID_UUID cesar::UUIDFactory::InvalidUUID()
#define CESAR_UUID_STRING(uuid) cesar::UUIDFactory::uuidToString(uuid)
#define CESAR_UUID_FROM_STRING(uuid) cesar::UUIDFactory::uuidFromString(uuid)

namespace cesar {
	class UUIDFactory;
	struct _declspec(dllexport) UUID
	{
		cesar::Uint64 high;
		cesar::Uint64 low;

		cesar::Bool operator==(const UUID& other) const
		{
			return high == other.high && low == other.low;
		}

		cesar::Bool operator!=(const UUID& other) const
		{
			return !(*this == other);
		}

		cesar::Bool operator<(const UUID& other) const
		{
			return (high < other.high) || (high == other.high && low < other.low);
		}

		operator bool()const;
	};

	class _declspec(dllexport) UUIDFactory
	{
		public:
			static UUID generateUUID();
			static inline constexpr UUID InvalidUUID() { return invalid; }

			static std::string uuidToString(const UUID& uuid);
			static UUID uuidFromString(const std::string& s);
		private:
			static constexpr inline UUID invalid{ 0,0 };
	};

	inline UUID::operator bool() const {
		return *this != UUIDFactory::InvalidUUID();
	}
}
namespace std
{
	template<>
	struct hash<cesar::UUID>
	{
		std::size_t operator()(const cesar::UUID& uuid) const noexcept
		{
			return std::hash<uint64_t>{}(uuid.high) ^ (std::hash<uint64_t>{}(uuid.low) << 1);
		}
	};
}
