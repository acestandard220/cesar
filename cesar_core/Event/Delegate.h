// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../Core/Types.h"

#include <functional>
#include <concepts>
#include <utility>

namespace cesar
{


#define DECLARE_DELEGATE(name,...)\
	using name = Delegate<void(__VA_ARGS__)>;

#define DECLARE_DELEGATE_RETVALUE(name, _return, ...)\
	using name = Delegate<_return(__VA_ARGS__)>;

#define DECLARE_MULTICAST_DELEGATE(name, ...)\
	using name = MultiCastDelegate<__VA_ARGS__>;

#define DECLARE_EVENT(name,owner,...)\
	class name : public MultiCastDelegate<__VA_ARGS__>\
	{\
		private:\
		friend class owner;\
		using MultiCastDelegate<__VA_ARGS__>::Broadcast;\
		using MultiCastDelegate<__VA_ARGS__>::RemoveAll;\
		using MultiCastDelegate<__VA_ARGS__>::Remove;\
	};


	template<typename...>
	class Delegate;

	template<typename DelegateType, typename Func>
	concept IsConstructible = std::is_constructible_v<DelegateType, Func>;

	template<typename _Return,typename ...Args>
	class Delegate<_Return(Args...)>
	{
		public:
			using DelegateType = std::function<_Return(Args...)>;
			Delegate() = default;
			Delegate(const Delegate& other) = default;
			~Delegate() = default;

			template<typename _Func>
				requires IsConstructible<DelegateType, _Func>
			void BindLambda(_Func&& function)
			{
				callback = function;
			}

			void BindStaticFunction(_Return(*function)(Args...))
			{
				callback = function;
			}

			template<typename T>
			void BindMemberFunction(_Return(T::* function)(Args...), T& instance)
			{
				callback = [&instance, function](Args&&... args) mutable -> _Return {return (instance.*function)(std::forward<Args>(args)...); };
			}

			void Unbind()
			{
				callback = nullptr;
			}

			_Return Execute()
			{
				return callback();
			}

			_Return ExecuteIfBound(Args... args)
			{
				return IsBound() ? callback(args...) : _Return();
			}

			Bool IsBound() const
			{
				return (callback) ? true : false;
			}

			template<typename _Func>
				requires IsConstructible<DelegateType, _Func>
			static Delegate CreateLambda(_Func&& function)
			{
				Delegate delegate;
				delegate.BindLambda(function);
				return delegate;
			}

			static Delegate CreateStaticFunction(_Return(*function)(Args...))
			{
				Delegate delegate;
				delegate.BindStaticFunction(function);
				return delegate;
			}

			template<typename T>
			static Delegate CreateMemberFunction(_Return(T::* function)(Args...), T& instance)
			{
				Delegate delegate;
				delegate.BindMemberFunction(function, instance);
				return delegate;
			}

			_Return operator()(Args... args)const
			{
				return callback(args...);
			}

		private:
			DelegateType callback = nullptr;
	};

	class DelegateHandle
	{
		public:
			DelegateHandle() :id(InvalidID) {};
			explicit DelegateHandle(int) :id(GenerateID()) {};
			~DelegateHandle() = default;

			void Reset()
			{
				id = InvalidID;
			}

			Bool IsValid()const
			{
				return (id != InvalidID);
			}

			operator bool()const
			{
				return (id != InvalidID);
			}
			
			Bool operator==(const DelegateHandle& other)const
			{
				return (id == other.id);
			}

		private:
			inline static const Uint64 InvalidID = Uint64(-1);
			static Uint64 GenerateID(){
				static Uint64 id_count = 0;
				return id_count++;
			}
			Uint64 id;
	};

	template<typename ...Args>
	class MultiCastDelegate
	{
		using DelegateType = Delegate<void(Args...)>;
		using HandleDelegatePair = std::pair<DelegateHandle, DelegateType>;

		public:
			MultiCastDelegate() = default;
			~MultiCastDelegate() = default;

			DelegateHandle Add(DelegateType&& delegate)
			{
				delegate_pool.emplace_back(DelegateHandle(0), std::forward<DelegateType>(delegate));
				return delegate_pool.back().first;
			}

			DelegateHandle Add(const DelegateType& delegate)
			{
				delegate_pool.emplace_back(DelegateHandle(0), delegate);
				return delegate_pool.back().first;
			}

			template<typename _Func>
			DelegateHandle AddLambda(_Func&& function)
			{
				return Add(DelegateType::CreateLambda(function));
			}

			template<typename _Return>
			DelegateHandle AddStaticFunction(_Return(*function)(Args...))
			{
				return Add(DelegateType::CreateStaticFunction(function));
			}

			template<typename T>
			DelegateHandle AddMemberFunction(void(T::* function)(Args...), T& instance)
			{
				return Add(DelegateType::CreateMemberFunction(function, instance));
			}

			Bool Remove(DelegateHandle& handle)
			{
				if (handle.IsValid())
				{
					for (Uint64 i = 0; i < delegate_pool.size(); i++)
					{
						if (delegate_pool[i].first == handle)
						{
							std::swap(delegate_pool[i], delegate_pool.back());
							delegate_pool.pop_back();
							handle.Reset();
							return true;
						}
					}
				}
				return false;
			}

			void RemoveAll()
			{
				delegate_pool.clear();
			}

			void Broadcast(Args... args)
			{
				for (Uint64 i = 0; i < delegate_pool.size(); i++) {
					if (delegate_pool[i].first)
					{
						delegate_pool[i].second.ExecuteIfBound(std::forward<Args>(args)...);
					}
				}
			}

		private:
			std::vector<HandleDelegatePair> delegate_pool;
	};
}