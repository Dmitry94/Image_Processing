#!/usr/bin/python

from __future__ import print_function
from __future__ import absolute_import
from __future__ import division


import captcha_model
import captcha_input as cap_in
import tensorflow as tf
from datetime import datetime


def main():
    coordinator = tf.train.Coordinator()
    manager = cap_in.CaptchaDataManager(captcha_model.BATCH_SIZE,
                                        captcha_model.NUMBERS,
                                        "train_data",
                                        coordinator, "NHWC")

    # Build a Graph that computes the logits predictions
    with tf.device("/CPU:0"):
        images, labels = manager.dequeue()

    logits = captcha_model.inference(images)
    tf.get_variable_scope().reuse_variables()
    loss = captcha_model.loss(logits, labels)
    train_op = captcha_model.train(loss)
    accuracy = captcha_model.evaluation(logits, labels)

    init = tf.group(tf.initialize_all_variables(),
                    tf.initialize_local_variables())
    sess = tf.Session(config=tf.ConfigProto(allow_soft_placement=True))
    sess.run(init)
    coord = tf.train.Coordinator()
    threads = manager.start_threads(sess)
    start_time = datetime.now()
    try:
        index = 1
        while not coord.should_stop():
            _, loss_value = sess.run([train_op, loss])
            print("step: " + str(index) + " loss:" + str(loss_value))
            if index % 5 == 0:
                validation_accuracy = sess.run(accuracy)
                print("validation accuracy: "+str(validation_accuracy))
            index += 1
    except tf.errors.OutOfRangeError:
        print('Done training -- epoch limit reached')
        end_time = datetime.now()
        print("Time Consumption: " + str(end_time - start_time))
    except KeyboardInterrupt:
        print("keyboard interrupt detected, stop running")
        del sess

    finally:
        sess.run(manager.queue.close(cancel_pending_enqueues=True))
        coordinator.request_stop()
        coordinator.join(threads)

    # Wait for threads to finish.
    coord.join(threads)
    sess.close()
    del sess


if __name__ == "__main__":
    main()
