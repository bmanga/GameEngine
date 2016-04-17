#pragma once

#include "MPL/MPL.h"

#include <vector>
#include <bitset>
#include <tuple>
#include <cassert>

#define ECS_TEST

#ifdef ECS_TEST
#include "position_component.h"
#include "render_component.h"
#include "light_component.h"
#include <gsl.h>
#endif

#define ECS_FWD(t) std::forward<decltype(t)> (t)

// Defines a "strong" type alias for `mUnderlying`, called `mName`.
#define ECS_STRONG_TYPEDEF(mUnderlying, mName)                                  \
    class mName final                                                           \
    {                                                                           \
    private:                                                                    \
        mUnderlying value;                                                      \
                                                                                \
    public:                                                                     \
        mName() = default;                                                      \
        mName(const mName& mX) = default;                                       \
        mName(mName&& mX) = default;                                            \
        mName& operator=(const mName& rhs) = default;                           \
        mName& operator=(mName&& rhs) = default;                                \
        const explicit mName(mUnderlying mX) noexcept : value{mX} {}	    	\
        const mName& operator=(mUnderlying rhs) noexcept				    	\
        {                                                                       \
            value = rhs;                                                        \
            return *this;                                                       \
        }                                                                       \
        operator const mUnderlying&() const noexcept							\
        {                                                                       \
            return value;                                                       \
        }                                                                       \
        operator mUnderlying&() noexcept { return value; }						\
        bool operator==(const mName& rhs) noexcept								\
        {                                                                       \
            return value == rhs.value;                                          \
        }                                                                       \
        bool operator!=(const mName& rhs) noexcept								\
        {                                                                       \
            return value != rhs.value;                                          \
        }                                                                       \
        bool operator<(const mName& rhs) noexcept								\
        {                                                                       \
            return value < rhs.value;                                           \
        }                                                                       \
        bool operator>(const mName& rhs) noexcept								\
        {                                                                       \
            return value > rhs.value;                                           \
        }                                                                       \
        bool operator<=(const mName& rhs) noexcept								\
        {                                                                       \
            return value <= rhs.value;                                          \
        }                                                                       \
        bool operator>=(const mName& rhs) noexcept								\
        {                                                                       \
            return value >= rhs.value;                                          \
        }                                                                       \
    };                                                                          \
                                                                                \
    static_assert(std::is_literal_type<mName>{}, "")

namespace Lemur{
namespace ecs{

// "Strong typedefs" used here to make sure the types are actually different.
// Allows function overloads and prevents conversion mistakes.

// Index of "real" data in component storage
ECS_STRONG_TYPEDEF(std::size_t, DataIndex);
// Index of entity "metadata", stored in contiguous structure
ECS_STRONG_TYPEDEF(std::size_t, EntityIndex);
ECS_STRONG_TYPEDEF(std::size_t, HandleDataIndex);
ECS_STRONG_TYPEDEF(int, Counter);

namespace Impl
{

template<typename TSettings>
struct Entity
{
	using Settings = TSettings;
	using Bitset = typename Settings::Bitset;

	// Index of data in the components storage
	DataIndex data_index;

	HandleDataIndex handle_data_index;

	// Stores components and tags
	Bitset bitset;
	bool alive;
};

struct HandleData
{
	EntityIndex entity_index;
	Counter counter;
};

struct Handle
{
	HandleDataIndex handle_data_index;
	Counter counter;
};

template<typename TSettings>
class ComponentStorage
{
private:
	using Settings = TSettings;
	using ComponentList = typename Settings::ComponentList;

	template<typename... Ts>
	using TupleOfVectors = std::tuple<std::vector<Ts>...>;

	BMPL::Rename<ComponentList, TupleOfVectors> vectors;

public:
	void grow(std::size_t new_capacity)
	{
		BMPL::ForEachInTuple([new_capacity](auto& v)
		{
			v.resize(new_capacity);
		}, vectors);
	}

	template<typename T>
	auto& getComponent(DataIndex i) noexcept
	{
		return std::get<std::vector<T>>(vectors)[i];
	}
};

} // namespace Impl

template<typename... Ts> using Signature = BMPL::Typelist<Ts...>;
template<typename... Ts> using SignatureList = BMPL::Typelist<Ts...>;
template<typename... Ts> using ComponentList = BMPL::Typelist<Ts...>;
template<typename... Ts> using TagList = BMPL::Typelist<Ts...>;

namespace Impl
{

template<typename TSettings>
struct SignatureBitsets;

template<typename TSettings>
class SignatureBitsetsStorage;

} // namespace Impl

template<
	typename TComponentList,
	typename TTagList,
	typename TSignatureList
>
struct Settings
{
	using ComponentList = typename TComponentList::Typelist;
	using TagList = typename TTagList::Typelist;
	using SignatureList = typename TSignatureList::Typelist;
	using ThisType = Settings<ComponentList, TagList, SignatureList>;

	using SignatureBitsets = Impl::SignatureBitsets<ThisType>;
	using SignatureBitsetsStorage =
		Impl::SignatureBitsetsStorage<ThisType>;

	template<typename T>
	static constexpr bool isComponent() noexcept
	{
		return BMPL::Contains<ComponentList, T>;
	}
	template<typename T>
	static constexpr bool isTag() noexcept
	{
		return BMPL::Contains<TagList, T>;
	}
	template<typename T>
	static constexpr bool isSignature() noexcept
	{
		// FIX: DIRTY HACK
		return BMPL::Contains<SignatureList, BMPL::Typelist<T>>;
	}

	static constexpr std::size_t componentCount() noexcept
	{
		return ComponentList::size;
	}
	static constexpr std::size_t tagCount() noexcept
	{
		return TagList::size;
	}
	static constexpr std::size_t signatureCount() noexcept
	{
		return SignatureList::size;
	}

	template<typename TComponent>
	static constexpr std::size_t componentId() noexcept
	{
		return BMPL::impl::IndexOf<ComponentList, TComponent>::value;

	}

	template<typename TTag>
	static constexpr std::size_t tagId() noexcept
	{
		return BMPL::impl::IndexOf<TagList, TTag>::value;
	}

	template<typename TSignature>
	static constexpr std::size_t signatureId() noexcept
	{
		//constexpr size_t index = SignatureList::indexOf<T>;
		return BMPL::impl::IndexOf<SignatureList, TSignature>::value;
		
	}

	using Bitset = std::bitset<ThisType::componentCount() + ThisType::tagCount()>;

	template<typename TComponent>
	static constexpr std::size_t componentBit() noexcept
	{
		return componentId<TComponent>();
	}
	template<typename TTag>
	static constexpr std::size_t tagBit() noexcept
	{
		return componentCount() + tagId<TTag>();
	}
};

namespace Impl
{

template<typename TSettings>
struct SignatureBitsets
{
	using Settings = TSettings;
	using ThisType = SignatureBitsets<Settings>;
	using SignatureList = typename Settings::SignatureList;
	using Bitset = typename Settings::Bitset;

	using BitsetStorage = BMPL::TupleType<
		BMPL::Repeat<Settings::signatureCount(), Bitset>
	>;

	template<typename T>
	using IsComponentFilter = std::integral_constant<
		bool, Settings::template isComponent<T>()
	>;

	template<typename T>
	using IsTagFilter = std::integral_constant<
		bool, Settings::template isTag<T>()
	>;

	template<typename TSignature>
	using SignatureComponents = BMPL::Filter<
		TSignature,
		IsComponentFilter
	>;

	template<typename TSignature>
	using SignatureTags = BMPL::Filter<
		TSignature,
		IsTagFilter
	>;
};

template<typename TSettings>
class SignatureBitsetsStorage
{
private:
	using Settings = TSettings;
	using SignatureBitsets = typename Settings::SignatureBitsets;
	using SignatureList = typename SignatureBitsets::SignatureList;
	using BitsetStorage = typename SignatureBitsets::BitsetStorage;

	BitsetStorage storage;

public:
	template<typename TSignature>
	auto& getSignatureBitset() noexcept
	{
		static_assert(Settings::template isSignature<TSignature>(), "");
		return std::get<Settings::template signatureId<TSignature>()>(storage);
	}

	template<typename TSignature>
	const auto& getSignatureBitset() const noexcept
	{
		static_assert(Settings::template isSignature<TSignature>(), "");
		return std::get<Settings::template signatureId<TSignature>()>(storage);
	}

private:
	template<typename TSignature>
	void initializeBitset() noexcept
	{
		auto& b(this->getSignatureBitset<TSignature>());

		using SignatureComponents = typename SignatureBitsets::template SignatureComponents<TSignature>;

		using SignatureTags = typename SignatureBitsets::template SignatureTags<TSignature>;

		// FIX: Make this not so hacky
		BMPL::ForEachTypeAsArg<SignatureComponents>([this, &b](auto t)
		{
			b[Settings::template componentBit<BMPL_TYPE(t)>()] = true;
		});

		// FIX: Make this not so hacky
		BMPL::ForEachTypeAsArg<SignatureTags>([this, &b](auto t)
		{
			b[Settings::template tagBit<BMPL_TYPE(t)>()] = true;
		});
	}

public:
	SignatureBitsetsStorage() noexcept
	{
		// FIX: Make this not so hacky
		BMPL::ForEachTypeAsArg<SignatureList>([this](auto t)
		{
			this->initializeBitset<BMPL_TYPE(t)>();
		});
	}
};

} // namespace Impl

template<typename TSettings>
class Manager
{
private:
	// Type aliases
	using Settings = TSettings;
	using ThisType = Manager<Settings>;
	using Bitset = typename Settings::Bitset;
	using Entity = Impl::Entity<Settings>;
	using HandleData = Impl::HandleData;

public:
	using Handle = Impl::Handle;

private:
	using SignatureBitsetsStorage =
		Impl::SignatureBitsetsStorage<Settings>;
	using ComponentStorage = Impl::ComponentStorage<Settings>;

	// Storage capacity (memory is allocated for 'capacity' entities)
	std::size_t capacity{ 0 };

	// Current size
	// Does not take into account newly-created entities.
	std::size_t size{ 0 };

	// Next size (after refresh)
	// Takes into account newly-created entities.
	std::size_t size_next{ 0 };

	std::vector<Entity> entities;
	SignatureBitsetsStorage signature_bitsets;
	ComponentStorage components;
	std::vector<HandleData> handle_data;

	void growTo(std::size_t new_capacity)
	{
		assert(new_capacity > capacity);

		entities.resize(new_capacity);
		components.grow(new_capacity);
		handle_data.resize(new_capacity);

		for (auto i(capacity); i < new_capacity; ++i)
		{
			auto& e(entities[i]);
			auto& h(handle_data[i]);

			e.data_index = i;
			e.bitset.reset();
			e.alive = false;
			e.handle_data_index = i;

			h.counter = 0;
			h.entity_index = i;
		}

		capacity = new_capacity;
	}

	void growIfNeeded()
	{
		if (capacity > size_next) return;
		growTo((capacity + 10) * 2);
	}

	auto& getEntity(EntityIndex i) noexcept
	{
		assert(size_next > i);
		return entities[i];
	}

	const auto& getEntity(EntityIndex i) const noexcept
	{
		assert(size_next > i);
		return entities[i];
	}

	auto& getHandleData(HandleDataIndex i) noexcept
	{
		assert(handle_data.size() > i);
		return handle_data[i];
	}
	const auto& getHandleData(HandleDataIndex i) const noexcept
	{
		assert(handle_data.size() > i);
		return handle_data[i];
	}

	auto& getHandleData(EntityIndex i) noexcept
	{
		return getHandleData(getEntity(i).handle_data_index);
	}
	const auto& getHandleData(EntityIndex i) const noexcept
	{
		return getHandleData(getEntity(i).handle_data_index);
	}

	auto& getHandleData(const Handle& x) noexcept
	{
		return getHandleData(x.handle_data_index);
	}
	const auto& getHandleData(const Handle& x) const noexcept
	{
		return getHandleData(x.handle_data_index);
	}

public:
	Manager()
	{
		growTo(100);
	}

	auto isValidHandle(const Handle& x) const noexcept
	{
		return x.counter == getHandleData(x).counter;
	}

	auto getEntityIndex(const Handle& x) const noexcept
	{
		assert(isValidHandle(x));
		return getHandleData(x).entity_index;
	}

	auto isAlive(EntityIndex i) const noexcept
	{
		return getEntity(i).alive;
	}
	auto isAlive(const Handle& x) const noexcept
	{
		return isAlive(getEntityIndex(x));
	}

	void kill(EntityIndex i) noexcept
	{
		getEntity(i).alive = false;
	}
	void kill(const Handle& x) noexcept
	{
		kill(getEntityIndex(x));
	}

	template<typename TTag>
	auto hasTag(EntityIndex i) const noexcept
	{
		static_assert(Settings::template isTag<TTag>(), "");
		return getEntity(i).bitset[Settings::template tagBit<TTag>()];
	}
	template<typename TTag>
	auto hasTag(const Handle& x) const noexcept
	{
		return hasTag<TTag>(getEntityIndex(x));
	}

	template<typename TTag>
	void addTag(EntityIndex i) noexcept
	{
		static_assert(Settings::template isTag<TTag>(), "");
		getEntity(i).bitset[Settings::template tagBit<TTag>()] = true;
	}
	template<typename TTag>
	void addTag(const Handle& x) noexcept
	{
		return addTag<TTag>(getEntityIndex(x));
	}

	template<typename TTag>
	void delTag(EntityIndex i) noexcept
	{
		static_assert(Settings::template isTag<TTag>(), "");
		getEntity(i).bitset[Settings::template tagBit<TTag>()] = false;
	}
	template<typename TTag>
	auto delTag(const Handle& x) noexcept
	{
		return delTag<TTag>(getEntityIndex(x));
	}

	template<typename TComponent>
	auto hasComponent(EntityIndex i) const noexcept
	{
		static_assert(Settings::template isComponent<TComponent>(), "");
		return getEntity(i).bitset[Settings::template componentBit<TComponent>()];
	}
	template<typename TComponent>
	auto hasComponent(const Handle& x) const noexcept
	{
		return hasComponent<TComponent>(getEntityIndex(x));
	}

	template<typename TComponent, typename... TArgs>
	auto& addComponent(EntityIndex i, TArgs&&... Xs) noexcept
	{
		static_assert(Settings::template isComponent<TComponent>(), "");

		auto& e(getEntity(i));
		e.bitset[Settings::template componentBit<TComponent>()] = true;

		auto& c(components.template getComponent<TComponent>(e.data_index));
		// NOTE: This used to be FWD :S
		new (&c) TComponent(ECS_FWD(Xs)...);

		return c;
	}
	template<typename TComponent, typename... TArgs>
	auto& addComponent(const Handle& x, TArgs&&... Xs) noexcept
	{
		// NOTE: This used to be FWD :S
		return addComponent<TComponent>(getEntityIndex(x), ECS_FWD(Xs)...);
	}

	template<typename TComponent>
	auto& getComponent(EntityIndex i) noexcept
	{
		static_assert(Settings::template isComponent<TComponent>(), "");
		assert(hasComponent<TComponent>(i));

		return components.template getComponent<TComponent>(getEntity(i).data_index);
	}
	template<typename TComponent>
	auto& getComponent(const Handle& x) noexcept
	{
		return getComponent<TComponent>(getEntityIndex(x));
	}

	template<typename TComponent>
	void delComponent(EntityIndex i) noexcept
	{
		static_assert(Settings::template isComponent<TComponent>(), "");
		getEntity(i).bitset[Settings::template componentBit<TComponent>()] = false;
	}
	template<typename TComponent>
	void delComponent(const Handle& x) noexcept
	{
		return delComponent<TComponent>(getEntityIndex(x));
	}

	auto createIndex()
	{
		growIfNeeded();
		EntityIndex free_index(size_next++);

		assert(!isAlive(free_index));
		auto& e(entities[free_index]);
		e.alive = true;
		e.bitset.reset();

		return free_index;
	}

	auto createHandle()
	{
		auto free_index(createIndex());
		assert(isAlive(free_index));

		auto& e(entities[free_index]);
		auto& hd(handle_data[e.handle_data_index]);
		hd.entity_index = free_index;

		Handle h;
		h.handle_data_index = e.handle_data_index;
		h.counter = hd.counter;
		assert(isValidHandle(h));

		return h;
	}

	void clear() noexcept
	{
		for (auto i(0u); i < capacity; ++i)
		{
			auto& e(entities[i]);
			auto& hd(handle_data[i]);

			e.data_index = i;
			e.bitset.reset();
			e.alive = false;
			e.handle_data_index = i;

			hd.counter = 0;
			hd.entity_index = i;
		}

		size = size_next = 0;
	}

	void refresh() noexcept
	{
		if (size_next == 0)
		{
			size = 0;
			return;
		}

		size = size_next = refreshImpl();
	}

	template<typename TSignature>
	auto matchesSignature(EntityIndex i) const noexcept
	{
		static_assert(Settings::template isSignature<TSignature>(), "");

		const auto& entity_bitset(getEntity(i).bitset);
		const auto& signature_bitset(signature_bitsets.template getSignatureBitset<TSignature>());

		return (signature_bitset & entity_bitset) == signature_bitset;
	}

	template<typename TF>
	void forEntities(TF&& function)
	{
		for (EntityIndex i{ 0 }; i < size; ++i)
			function(i);
	}

	template<typename TSignature, typename TF>
	void forEntitiesMatching(TF&& function)
	{
		static_assert(Settings::template isSignature<TSignature>(), "");

		forEntities([this, &function](auto i)
		{
			if (matchesSignature<TSignature>(i))
				expandSignatureCall<TSignature>(i, function);
		});
	}

private:
	template<typename... Ts>
	struct ExpandCallHelper;

	template<typename TSignature, typename TF>
	void expandSignatureCall(EntityIndex i, TF&& function)
	{
		static_assert(Settings::template isSignature<TSignature>(), "");

		using RequiredComponents = typename Settings::SignatureBitsets::template SignatureComponents<TSignature>;

		using Helper = BMPL::Rename<
			RequiredComponents, ExpandCallHelper
		>;

		Helper::call(i, *this, function);
	}

	template<typename... TComponents>
	struct ExpandCallHelper
	{
		template<typename TF>
		static void call(EntityIndex i, ThisType& mgr, TF&& function)
		{
			auto di(mgr.getEntity(i).data_index);

			function(
				i,
				mgr.components.template getComponent<TComponents>(di)...
			);
		}
	};

private:
	void invalidateHandle(EntityIndex x) noexcept
	{
		auto& hd(handle_data[entities[x].handle_data_index]);
		++hd.counter;
	}

	void refreshHandle(EntityIndex x) noexcept
	{
		auto& hd(handle_data[entities[x].handle_data_index]);
		hd.entity_index = x;
	}

	// Core refreshing algorithm.
	//
	// Algorithm implemented using two indices:
	//	 - 'iD' looks for dead entities, starting from the left.
	//	 - 'iA' looks for alive entities, starting from the right.
	//
	// Newly-created entities will always be near the end of the vector.
	//
	// Alive entities found on the right will be swapped with dead entities
	// found on the left - this will re-arrange the entities as intended.
	//
	// Particular care must be taken to avoid off-by-one errors!
	//
	// The function will return the number of alive entities, which is
	// one-past the index of the last alive entity.
	auto refreshImpl() noexcept
	{
		EntityIndex iD{ 0 }, iA{ size_next - 1 };

		while (true)
		{
			// Find first dead entity from the left
			for (; true; ++iD)
			{
				// Immediately check if iD index has gone through iA index.
				// If that's the case, there are no more dead entities in the
				// vector and iD is returned as the result.
				if (iD > iA) return iD;

				// If a dead entity was found, break out of inner for-loop.
				if (!entities[iD].alive) break;
			}

			// Find first alive entity from the right.
			for (; true; --iA)
			{
				// If an alive entity was found, break out of inner for-loop.
				if (entities[iA].alive) break;

				// Otherwise, acknowledge entity that has been killed since last
				// refresh.
				invalidateHandle(iA);

				// If iD reached, there are no more alive entities in the entity
				// metadata vector, and iD is returned.
				if (iA <= iD) return iD;
			}

			// At this point, two entities have been found that need to be swapped.

			// Ensure iA points to an alive entity, towards right of vector.
			assert(entities[iA].alive);

			// Ensure iD points to a dead entity, towards left of vector.
			assert(!entities[iD].alive);

			// Swap them to arrange all alive entities towards the left.
			std::swap(entities[iA], entities[iD]);

			// Refresh the alive entity's handle and invalidate the dead swapped
			// entity's handle.
			refreshHandle(iD);
			invalidateHandle(iA);
			refreshHandle(iA);

			// Move both "iterator" indices
			++iD; --iA;
		}

		return iD;
	}

public:
	auto getEntityCount() const noexcept
	{
		return size;
	}

	auto getCapacity() const noexcept
	{
		return capacity;
	}

	auto printState(std::ostream& oss) const
	{
		oss << "\nsize: " << size
			<< "\nsize_next: " << size_next
			<< "\ncapacity: " << capacity
			<< "\nstatus: ";

		for (auto i(0u); i < size_next; ++i)
		{
			auto& e(entities[i]);
			oss << (e.alive ? "A" : "D");
		}

		oss << "\n\n";
	}
};

} // namespace ecs

using MyComponents = Lemur::ecs::ComponentList
<
	CPosition, CRenderable, CLight
>;

struct TTestTag {};
struct TTestTag2 {};
using MyTags = Lemur::ecs::TagList
<
	TTestTag, TTestTag2
>;

using SRenderable = Lemur::ecs::Signature<CPosition, CRenderable>;
using SLight = Lemur::ecs::Signature<CPosition, CLight>;
using MySignatures = Lemur::ecs::SignatureList
<
	SRenderable, SLight
>;

using MySettings = Lemur::ecs::Settings
<
	MyComponents, MyTags, MySignatures
>;

using MyManager = Lemur::ecs::Manager<MySettings>;

} // namespace Lemur