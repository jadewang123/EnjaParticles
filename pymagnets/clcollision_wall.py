import pyopencl as cl

class CLCollisionWall:
    def __init__(self, clsph):
        self.clsph = clsph
        self.queue = self.clsph.queue
        self.dt = self.clsph.dt
        self.clsph.loadProgram(self.clsph.clsph_dir + "/collision_wall.cl")
    
    #@timings
    def execute(self, num, *args, **argv):
        if num > 0:
            worksize = 128
            factor = 1.*num / worksize
            if int(factor) != factor:
                factor = int(factor)
                global_size = (worksize * factor + worksize,)
            else:
                global_size = (num,)
            local_size = (worksize,)

            self.clsph.prgs["collision_wall"].collision_wall(self.queue, global_size, local_size, *(args))

            self.queue.finish()
     
