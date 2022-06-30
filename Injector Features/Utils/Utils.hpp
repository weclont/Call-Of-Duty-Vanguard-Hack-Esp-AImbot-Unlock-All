#pragma once

#ifndef STD_INCLUDED
#error "Missing standard header"
#endif

namespace Utils
{
	class StaticInitializer
	{
	public:
		StaticInitializer(std::function<void()> init, std::function<void()> _uninit = std::function<void()>())
		{
			if (init) init();
			this->uninit = _uninit;
		}

		~StaticInitializer()
		{
			if (this->uninit) this->uninit();
		}

	private:
		std::function<void()> uninit;
	};

	class Buffer : public std::string
	{
	public:
		Buffer() : std::string() {}
		Buffer(std::string data) : Buffer()
		{
			this->append(data);
		}

		template <typename T> void write(T data)
		{
			this->append(reinterpret_cast<char*>(&data), sizeof T);
		}

		template <typename T> T read()
		{
			T data;
			if (!this->read(&data)) throw std::runtime_error("Buffer overflow");
			return data;
		}

		template <typename T> bool read(T* data)
		{
			if (this->size() < sizeof T) return false;

			std::memmove(data, this->data(), sizeof T);
			this->erase(this->begin(), this->begin() + sizeof T);

			return true;
		}
	};

	template <typename T> inline void Merge(std::vector<T>* target, T* source, size_t length)
	{
		if (source)
		{
			target->reserve(target->size() + length);
			for (size_t i = 0; i < length; ++i)
			{
				target->push_back(source[i]);
			}
		}
	}

	template <typename T> inline void Merge(std::vector<T>* target, std::vector<T> source)
	{
		target->reserve(target->size() + source.size());
		for (auto &entry : source)
		{
			target->push_back(entry);
		}
	}

	void SetEnvironment();
	std::string LoadResource(int resId);
}