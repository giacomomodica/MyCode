
class queue_checker {

public:
	queue_checker(MessageQueue * mqptr, virtual_machine *this_vm) {
		the_mq = mqptr;
		vm = this_vm;
	};

	void operator()() const {
		while (true) {
			string message;
			the_mq->wait_and_pop(message);
			lock_guard<mutex> guard(vm->glob_mutex);
			if (message == "run") {
				vm->run_now = true;
			}
			else if (message == "stop") {
				vm->run_now = false;
			}
			else if (message == "exit") {
				vm->exit_code = true;
				break;
			}
		}
	}

private:
	MessageQueue * the_mq;
	virtual_machine *vm;
};