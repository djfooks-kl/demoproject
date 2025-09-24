#pragma once

#include <string>
#include <variant>

namespace xc
{
	struct ResultError
	{
		ResultError() {}
		explicit ResultError(std::string error) : m_Error(std::move(error)) {}

		std::string m_Error;
	};

	template<typename TResult>
	class Result
	{
	public:
		Result(TResult result)
			: m_Value(std::move(result))
		{
		}

		Result(xc::ResultError error)
			: m_Value(std::move(error))
		{}

		bool IsSuccess() const { return std::holds_alternative<TResult>(m_Value); }
		bool IsError() const { return !IsSuccess(); }

		operator bool() const { return IsSuccess(); }

		const TResult& Get() const { return std::get<TResult>(m_Value); }
		const std::string& GetError() const { return std::get<xc::ResultError>(m_Value).m_Error; }

	private:
		std::variant<TResult, xc::ResultError> m_Value;
	};

	template<>
	class Result<void>
	{
	public:
		Result()
			: m_Success(true)
		{
		}

		Result(xc::ResultError error)
			: m_Error(std::move(error))
		{
		}

		bool IsSuccess() const { return m_Success; }
		bool IsError() const { return !IsSuccess(); }

		operator bool() const { return m_Success; }

		const std::string& GetError() const { return m_Error.m_Error; }

	private:
		bool m_Success = false;
		xc::ResultError m_Error;
	};
}