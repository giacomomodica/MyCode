
class Worker {

public:
	Worker(virtual_machine *the_vm) {
		vm = the_vm;
	}

	void operator()() const {
		while (true) {
			vm->glob_mutex.lock();
			while (vm->run_now) {
				vm->step();
			}
			vm->glob_mutex.unlock();
		}
	}

private:
	virtual_machine * vm;
};