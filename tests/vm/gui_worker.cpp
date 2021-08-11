
class gui_Worker {

public:
	gui_Worker(MessageQueue * mqptr, VirtualMachineGUI *vm_gui) {
		the_mq = mqptr;
		the_vm_gui = vm_gui;
	};

	void operator()() const {
		while (true) {
			string message;
			the_mq->wait_and_pop(message);
			lock_guard<mutex> guard(the_vm_gui->glob_mutex);
			if (message == "run") {
				std::cout << "Recieved message from vm_gui, message was: " << message << endl;
				the_vm_gui->run_now = true;
			}
			else if (message == "stop") {
				the_vm_gui->run_now = false;
			}
			else if (message == "exit") {
				the_vm_gui->end_code = true;
				break;
			}
		}
	}

private:
	MessageQueue * the_mq;
	VirtualMachineGUI *the_vm_gui;
};