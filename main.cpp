#include "main.h"

Motor motor1(1);
Motor motor2(2);
Motor motor3(3);

Controller master(E_CONTROLLER_MASTER);

class Timer{
public:
	double delta_time() {
		return millis() - start_time;
	}
	void reset() {
		start_time = millis();
	}
	Timer() {
		start_time = 0;
	}
private:
	double start_time;
};

class Recorder {
public:
	bool recording;
	Recorder() {
		recording = false;
	}
	void start_recording() {
		Record.open("/usd/Record.txt");
		recording = true;
		timer.reset();
		std::cout << "Starting Recording\n";
	}
	void update() {
		Record << timer.delta_time() << " ";
		Record << motor1.get_voltage() << " ";
		Record << motor2.get_voltage() << " ";
		Record << motor3.get_voltage() << "\n";
	}
	void end_recording() {
		Record << "-1\n";
		timer.reset();
		Record.close();
		recording = false;
		std::cout << "Ending Recording\n";
		std::cout << "Recording Length : " << timer.delta_time() << "\n";
	}
	void start_playback() {
		Playback.open("/usd/Record.txt");
		playback();
	}
	void playback() {
		Playback >> play_time;
		if (play_time == -1) {
			Record.close();
			return;
		}
		// wait for the play_time to catch up to the recorded time
		while (timer.delta_time() < play_time) {}
		Playback >> motor_power;
		motor1 = motor_power;
		Playback >> motor_power;
		motor2 = motor_power;
		Playback >> motor_power;
		motor3 = motor_power;
		playback();
	}
private:
	Timer timer;
	std::ofstream Record;
	std::ifstream Playback;
	double play_time;
	double motor_power;
};

void initialize() {
	lcd::initialize();
	lcd::set_text(1, "Initializing File Recorder");
}

void disabled() {}

void competition_initialize() {}

void autonomous() {}

void opcontrol() {
	Recorder recorder;
	while (true) {

		if (master.get_digital_new_press(DIGITAL_A)) {
			recorder.start_recording();
		}

		if (master.get_digital_new_press(DIGITAL_B)) {
			recorder.end_recording();
		}

		if (master.get_digital(DIGITAL_L1)) {
			motor1 = 127;
		}
		else if (master.get_digital(DIGITAL_L2)) {
			motor1 = -127;
		}
		else {
			motor1 = 0;
		}

		if (master.get_digital(DIGITAL_R1)) {
			motor2 = 127;
		}
		else if (master.get_digital(DIGITAL_R2)) {
			motor2 = -127;
		}
		else {
			motor2 = 0;
		}

		motor3 = master.get_analog(ANALOG_LEFT_Y);

		if (master.get_digital_new_press(DIGITAL_X)) {
			recorder.start_playback();
		}

		pros::delay(20);
	}
}
