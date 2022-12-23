/**
 * @file asahi-smcd.c
 * @author Jason Conway (jpc@jasonconway.dev)
 * @date 2022-12-21
 * 
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MACSMC_BATTERY "/sys/class/power_supply/macsmc-battery"
#define MACSMC_BATTERY_CAPACITY MACSMC_BATTERY "/capacity"
#define MACSMC_BATTERY_BEHAVIOR MACSMC_BATTERY "/charge_behaviour"
#define MACSMC_BATTERY_STATUS   MACSMC_BATTERY "/status"

enum charge_behavior {
	BEHAVIOR_ERROR = -1,
	AUTO,
	INHIBIT_CHARGE,
	FORCE_DISCHARGE
};

enum charge_status {
	STATUS_ERROR = -1,
	CHARGING,
	DISCHARGING,
	NOTCHARGING,
	FULL
};

int get_capacity(void)
{
	FILE *file = fopen(MACSMC_BATTERY_CAPACITY, "rb");
	if (!file) {
		return -1;
	}

	char cap[4] = { 0 };
	if (!fread(cap, 1, sizeof(cap), file)) {
		fclose(file);
		return -1;
	}
	fclose(file);
	return strtol(cap, NULL, 10);
}

enum charge_status get_charge_status(void)
{
	FILE *file = fopen(MACSMC_BATTERY_STATUS, "rb");
	if (!file) {
		return STATUS_ERROR;
	}
	char status = '\0';
	if (!fread(&status, 1, 1, file)) {
		fclose(file);
		return STATUS_ERROR;
	}
	fclose(file);
	switch (status) {
		case 'C':
			return CHARGING;
		case 'D':
			return DISCHARGING;
		case 'N':
			return NOTCHARGING;
		case 'F':
			return FULL;
		default:
			return STATUS_ERROR;
	}
}

int set_charge_behavior(enum charge_behavior new)
{
	FILE *file = fopen(MACSMC_BATTERY_BEHAVIOR, "wb");
	if (!file) {
		return 1;
	}

	static const char *behaviors[] = { "auto", "inhibit-charge", "force-discharge" };
	const char *behavior = behaviors[new];
	const size_t len = strlen(behavior) + 1;
	if (fwrite(behavior, 1, len, file) != len) {
		fclose(file);
		return 1;
	}
	return fclose(file);
}

int main(int argc, char const *argv[])
{
	int charge_level = 80;
	if (argc > 1) {
		long level = strtol(argv[1], NULL, 10);
		if (level > 0 && level <= 100) {
			charge_level = level;
		}
	}

	for (int err = 0; !err; sleep(30)) {
		const int charge = get_capacity();
		const enum charge_status status = get_charge_status();
		if (charge < 0 || charge > 100 || status == STATUS_ERROR) {
			err = -1;
		}
		else if (charge > charge_level && status != DISCHARGING) {
			err = set_charge_behavior(FORCE_DISCHARGE);
		}
		else if (charge < charge_level && status != CHARGING) {
			err = set_charge_behavior(AUTO);
		}
		else if (charge == charge_level && status != NOTCHARGING) {
			err = set_charge_behavior(INHIBIT_CHARGE);
		}
	}
}
