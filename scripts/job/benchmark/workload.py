from base import job_desc_pb2
from base import task_desc_pb2
from base import reference_desc_pb2
from base import resource_vector_pb2
from google.protobuf import text_format
import httplib, urllib, re, sys, random
import binascii
import time
import shlex
from job import *
from task import *
from mesos_job import *

class Workload:
  def __init__(self, host, port, target):
    self.jobs = {}
    self.scheduler_host = host
    self.scheduler_port = port
    self.target = target

  def add(self, name, binary, tasks_args, task_count,
          task_type=task_desc_pb2.TaskDescriptor.TURTLE,
          resource_request=None):
    if self.target == "firmament":
      self.jobs[name] = Job(name)
    elif self.target == "mesos":
      self.jobs[name] = MesosJob(name)
    else:
      print "ERROR: Unexpected target %s" % (self.target)
      return
    self.jobs[name].prepare(binary, tasks_args, task_count, task_type=task_type,
                            resource_request=resource_request)

  def start(self):
    self.start_time = time.time()
    print "Starting workload composed of the following jobs:"
    for jn, j in sorted(self.jobs.items(), key=lambda x: random.random()):
      print "- %s" % (jn),
      (success, job_id) = j.submit(self.scheduler_host, self.scheduler_port)
      if success:
        print "... running (%s)" % (job_id)
      else:
        print "... ERROR"
      time.sleep(1)

  def restart_completed(self):
    for jn, j in sorted(self.jobs.items(), key=lambda x: random.random()):
      if j.completed(self.scheduler_host, self.scheduler_port):
        print "- %s" % (jn),
        j.instance += 1
        (success, job_id) = j.submit(self.scheduler_host, self.scheduler_port)
        if success:
          print "... re-submitted (%s)" % (job_id)
        else:
          print "... ERROR"
        time.sleep(1)
