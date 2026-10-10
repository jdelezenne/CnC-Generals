#pragma once

enum waveType
{
	WaveTypeFirst,
	WaveTypePond=WaveTypeFirst,
	WaveTypeOcean,
	WaveTypeCloseOcean,	//same as above but appears much closer to beach.
	WaveTypeCloseOceanDouble,	//same as above but waves much sloser together.
	WaveTypeRadial,
	WaveTypeLast = WaveTypeRadial,
	WaveTypeStationary,
	WaveTypeMax,
};
