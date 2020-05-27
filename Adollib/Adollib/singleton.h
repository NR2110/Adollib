#pragma once

namespace Adollib
{
	// ****************************************
	// Singleton (create-destroy�Ő����j��, �Q�ƕԂ�)
	// ****************************************
	template <class T>
	class Singleton
	{
	private:
		static T* _instance;
	public:
		// ����
		static void create()
		{
			if (_instance == nullptr)
			{
				_instance = new T;
			}
		}
		// �j��
		static void destroy()
		{
			if (_instance != nullptr)
			{
				delete _instance;
				_instance = nullptr;
			}
		}
		// �C���X�^���X�̎Q�Ƃ��擾
		static T& getInstance()
		{
			return *_instance;
		}
		// �C���X�^���X�̃|�C���^���擾
		static T* getInstancePtr()
		{
			return _instance;
		}
		// �C���X�^���X�����݂��邩
		static bool isCreate()
		{
			return _instance != nullptr;
		}
		// �C���X�^���X���j������Ă��邩
		static bool isDestroy()
		{
			return _instance == nullptr;
		}
	protected:
		Singleton() = default;
		virtual ~Singleton() = default;

		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;
		Singleton(Singleton&&) = delete;
		Singleton& operator=(Singleton&&) = delete;

	};

	template<class T>
	T* Singleton<T>::_instance = nullptr;
}