#include "emu.h"
#include "toaplan2.h"
#include "toaplipt.h"

#include "gp9001.h"

#include "cpu/nec/v25.h"
#include "cpu/z80/z80.h"
#include "cpu/z180/hd647180x.h"
#include "machine/nvram.h"
#include "sound/ymopm.h"
#include "sound/ymopl.h"
#include "sound/ymz280b.h"
#include "speaker.h"


// with text layer
class truxton2_state : public driver_device
{
public:
	truxton2_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)

	{ }

};
