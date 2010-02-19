// Copyright 2010, Thomas Walters
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
%include "std_string.i"
%{
#include "Support/Common.h"
#include "Support/Module.h"
#include "Support/Parameters.h"
#include "Support/SignalBank.h"
#include "Modules/Features/ModuleGaussians.h"
#include "Modules/BMM/ModuleGammatone.h"
%}

namespace aimc {
class Parameters {
public:
	Parameters();
	~Parameters();
	bool Load(const char *sParamFilename);
	bool Save(const char *sParamFilename);
  std::string WriteString();
	bool Merge(const char *sParamFilename);
	const char* DefaultString(const char *sName, const char *val);
	int DefaultInt(const char *sName, int val);
	unsigned int DefaultUInt(const char *sName, unsigned int val);
	float DefaultFloat(const char *sName, float val);
	bool DefaultBool(const char *sName, bool val);
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
    ~SignalBank();
    bool Initialize(int channel_count, int signal_length, float sample_rate);
    bool Initialize(const SignalBank &input);
    bool Validate() const;
    const vector<float> &operator[](int channel) const;
    float sample(int channel, int index) const;
    void set_sample(int channel, int index, float value);
    float sample_rate() const;
    int buffer_length() const;
    int start_time() const;
    void set_start_time(int start_time);
    float centre_frequency(int i) const;
    void set_centre_frequency(int i, float cf);
    bool initialized() const;
    int channel_count() const;
};

class Module {
 public:
  explicit Module(Parameters *parameters);
  virtual ~Module();
  bool Initialize(const SignalBank &input);
  bool initialized() const;
  bool AddTarget(Module* target_module);
  bool RemoveTarget(Module* target_module);
  void RemoveAllTargets();
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

class ModuleGammatone : public Module
{
 public:
	ModuleGammatone(Parameters *pParam);
	virtual ~ModuleGammatone();
	virtual void Process(const SignalBank &input);
	void Reset();
};
}  // namespace aimc

