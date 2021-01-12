#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include "Timer.h"


namespace FraplesDev
{
	namespace SpeedLog
	{
		class SpeedLog
		{
			struct Entry
			{
				Entry(std::string s, float t) 
					:_mLabel(std::move(s)),_mTime(t)
				{

				}
				void WriteTo(std::ostream& out)const noexcept
				{
					using namespace std::string_literals;
					if (_mLabel.empty())
					{
						out << _mTime * 1000.0f << "ms\n";
					}
					else
					{
						out << std::setw(16)<<std::left<<"["s +  _mLabel + "]"s <<std::setw(6)<<std::right<<_mTime * 1000.0f << "ms\n";
					}
				}
				std::string _mLabel;
				float _mTime;
			};
			static SpeedLog& Get_()noexcept
			{
				static SpeedLog log;
				return log;
			}
			SpeedLog()noexcept
			{
				_mEntries.reserve(3000);
			}
			~SpeedLog()noexcept
			{
				Flush_();
			}
			void Start_(const std::string& label = "")noexcept
			{
				_mEntries.emplace_back(label, 0.0f);
				_mTimer.Time();
			}
			void Mark_(const std::string& label = "")noexcept
			{
				float t = _mTimer.Get();
				_mEntries.emplace_back(label, t);
			}
			void Flush_()
			{
				std::ofstream file("perf.txt");
				file << std::setprecision(3) << std::fixed;
				for (const auto& e : _mEntries)
				{
					e.WriteTo(file);
				}
			}
		public:
			static void Start(const std::string& label = "")noexcept
			{
				Get_().Start_(label);
			}
			static void Mark(const std::string& label = "")noexcept
			{
				Get_().Mark_(label);
			}
		private:
			Timer _mTimer;
			std::vector<Entry>_mEntries;
		};

	}
}