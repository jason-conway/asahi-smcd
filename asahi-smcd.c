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
#include <stdbool.h>
#include <unistd.h>

#define SETPOINT 80

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

	char cap[3] = { 0 };
	if (!fread(cap, 1, sizeof(cap), file)) {
		fclose(file);
		return -1;
	}
	fclose(file);
	return 100 * (cap[0] - '0') + 10 * (cap[1] - '0') + (cap[2] - '0');
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

int main(void)
{
	bool run = true;
	while (run) {
		const int charge = get_capacity();
		const enum charge_status status = get_charge_status();
		if (charge < 0 || status == STATUS_ERROR) {
			break;
		}
		else if (charge > SETPOINT && status != DISCHARGING) {
			run = !set_charge_behavior(FORCE_DISCHARGE);
		}
		else if (charge < SETPOINT && status != CHARGING) {
			run = !set_charge_behavior(AUTO);
		}
		else if (charge == SETPOINT && status != NOTCHARGING) {
			run = !set_charge_behavior(INHIBIT_CHARGE);
		}
		sleep(30);
	}
	return set_charge_behavior(AUTO);
}
