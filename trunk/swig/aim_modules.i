// Copyright 2008-2010, Thomas Walters
//
// AIM-C: A C++ implementation of the Auditory Image Model
// http://www.acousticscale.org/AIMC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

%module aimc 
%{
#include "Support/Common.h"
#include "Support/Module.h"
#include "Support/Parameters.h"
#include "Support/SignalBank.h"
#include "Modules/Features/ModuleGaussians.h"
%}

namespace aimc {
class Parameters {
public:
	Parameters();
	~Parameters();
	bool Load(const char *sParamFilename);
	bool Save(const char *sParamFilename);
	bool Merge(const char *sParamFilename);
  void SetDefault(const char* sName, const char* value);
	void SetString(const char *sName, const char *val);
	void SetInt(const char *sName, int val);
	void SetUInt(const char *sName, unsigned int val);
	void SetFloat(const char *sName, float val);
	void SetBool(const char *sName, bool val);
	const char *GetString(const char *sName);
	int GetInt(const char *sName);
	unsigned int GetUInt(const char *sName);
	float GetFloat(const char *sName);
	bool GetBool(const char *sName);
	bool IsSet(const char *sName);
	bool Parse(const char *sCmd);
	bool Delete(const char *sName);
	static const unsigned int MaxParamNameLength = 128;
};

class SignalBank {
 public:
  SignalBank();
  bool Initialize(int channel_count, int signal_length, float sample_rate);
  bool Validate() const;
  inline const vector<float> &operator[](int channel) const;
  void set_sample(int channel, int sample, float value);
  float sample_rate() const;
  int buffer_length() const;
  int start_time() const;
  void set_start_time(int start_time);
  float get_centre_frequency(int i) const;
  void set_centre_frequency(int i, float cf);
  bool initialized() const;
  int channel_count() const;
 private:
  DISALLOW_COPY_AND_ASSIGN(SignalBank);
};

class Module {
 public:
  explicit Module(Parameters *parameters);
  virtual ~Module();
  bool Initialize(const SignalBank &input);
  bool initialized() const;
  bool AddTarget(Module* target_module);
  bool DeleteTarget(Module* target_module);
  void DeleteAllTargets();
  virtual void Process(const SignalBank &input) = 0;
  virtual void Reset() = 0;
  const SignalBank* GetOutputBank() const;
 private:
  DISALLOW_COPY_AND_ASSIGN(Module);
};

class ModuleGaussians : public Module
{
 public:
	ModuleGaussians(Parameters *pParam);
	virtual ~ModuleGaussians();
	virtual void Process(const SignalBank &input);
	void Reset();
};
}  // namespace aimc

