#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import argparse
import time
import string
import numpy as np

import captcha_input as cap_in
import cnn_model as model

import tensorflow as tf
from tensorflow.contrib import slim


ALPHABET = np.array(list(string.ascii_lowercase + string.digits))
CAPTCHA_SIZE = 5


def get_model_params(app_args):
    """
        Creating ModelParams object.
    """
    if app_args.data_format == "NCHW":
        data_format = "channels_first"
    else:
        data_format = "channels_last"
    model_params = model.ModelParams(
        filters_counts=app_args.filters_counts,
        conv_ksizes=app_args.conv_ksizes,
        conv_strides=app_args.conv_strides,
        pool_ksizes=app_args.pool_ksizes,
        pool_strides=app_args.pool_strides,
        fc_sizes=app_args.fc_sizes,
        drop_rates=app_args.drop_rates,
        data_format=data_format)

    return model_params


def train(app_args):
    """
      Train model for a number of steps.
    """

    with tf.Graph().as_default() as graph:
        coordinator = tf.train.Coordinator()
        manager = cap_in.CaptchaDataManager(app_args.batch_size,
                                            CAPTCHA_SIZE, app_args.hdf5_dir,
                                            coordinator, app_args.data_format)

        # Build a Graph that computes the logits predictions
        model_params = get_model_params(app_args)
        with tf.device("/CPU:0"):
            images, labels = manager.dequeue()
        im_shape = images.get_shape().as_list()[1:4]
        images = tf.placeholder_with_default(images, ([None] + im_shape),
                                             name="images")
        labels = tf.placeholder_with_default(labels, [None, CAPTCHA_SIZE],
                                             name="labels")
        logits = model.cnn_model(images, model_params)
        tf.add_to_collection('inputs', images)
        tf.add_to_collection('inputs', labels)
        tf.add_to_collection("logits", logits)

        # Calculate loss.
        logits = tf.reshape(logits, (app_args.batch_size,
                                     CAPTCHA_SIZE, len(ALPHABET)))
        cross_entropy_per_number = tf.nn.softmax_cross_entropy_with_logits(
            logits=logits, labels=labels)
        cross_entropy = tf.reduce_mean(cross_entropy_per_number)
        loss = tf.losses.get_total_loss() + cross_entropy

        # Set learning rate and optimizer
        global_step = tf.contrib.framework.get_or_create_global_step()
        num_batches_per_epoch = (manager.samples_count / app_args.batch_size)
        lr_decay_steps = app_args.num_epochs_lr_decay * num_batches_per_epoch
        lr = tf.train.exponential_decay(app_args.init_lr,
                                        global_step,
                                        lr_decay_steps,
                                        app_args.lr_decay_factor,
                                        staircase=True)
        opt = tf.train.GradientDescentOptimizer(lr)

        # Define ops
        init_op = tf.global_variables_initializer()
        train_op = slim.learning.create_train_op(loss, opt)
        prediction = tf.argmax(logits, 2)
        equal = tf.equal(tf.cast(prediction, tf.int32), labels)
        accuracy = tf.reduce_mean(tf.cast(equal, tf.float32), name="accuracy")

        tf.summary.scalar("Learning_rate", lr)
        tf.summary.scalar("Loss", loss)
        summary_op = tf.summary.merge_all()
        summary_writer = tf.summary.FileWriter(app_args.log_dir, graph)

        saver = tf.train.Saver()

        with tf.Session() as session:
            session.run(init_op)
            start_time = time.time()
            threads = manager.start_threads(session)

            for step in xrange(1, app_args.max_steps + 1):
                if step % app_args.log_frequency != 0:
                    session.run(train_op)
                else:
                    run_options = tf.RunOptions(
                        trace_level=tf.RunOptions.FULL_TRACE)
                    run_metadata = tf.RunMetadata()
                    _, loss_value, precission, summary = session.run(
                        [train_op, loss, accuracy, summary_op],
                        options=run_options,
                        run_metadata=run_metadata)

                    if (step % app_args.save_summary_steps == 0):
                        summary_writer.add_run_metadata(run_metadata,
                                                        "step%d" % step)
                        summary_writer.add_summary(summary, step)

                    current_time = time.time()
                    duration = current_time - start_time
                    start_time = current_time
                    loss_value = session.run(loss)
                    examples_per_sec = int(app_args.save_summary_steps *
                                           app_args.batch_size / duration)
                    sec_per_batch = float(duration /
                                          app_args.save_summary_steps)
                    print(
                        "Step = %d Loss = %f Precission = %f"
                        " Samples per sec = %d"
                        " Sec per batch = %f" %
                        (step, loss_value,
                         (np.sum(precission) / float(app_args.batch_size)),
                         examples_per_sec, sec_per_batch))

                if (step % app_args.save_checkpoint_steps == 0 or
                        step == app_args.max_steps):
                    checkpoint_file = os.path.join(app_args.log_dir,
                                                   "model.ckpt")
                    saver.save(session, checkpoint_file, step)
                    print("Checkpoint saved")

            session.run(manager.queue.close(cancel_pending_enqueues=True))
            coordinator.request_stop()
            coordinator.join(threads)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--hdf5-dir",
                        help="Path to the train directory",
                        default="train_data")

    parser.add_argument("--log-dir",
                        help="Path to the directory, where log will write",
                        default="cnn_train")

    parser.add_argument("--max-steps", type=int,
                        help="Number of batches to run",
                        default=100000)

    parser.add_argument("--batch-size", type=int,
                        help="Number of images to process in a batch",
                        default=128)

    parser.add_argument("--init-lr", type=float,
                        help="Start value for learning rate",
                        default=0.1)

    parser.add_argument("--lr-decay-factor", type=float,
                        help="Learning rate decay factor",
                        default=0.1)

    parser.add_argument("--num-epochs-lr-decay", type=int,
                        help="How many epochs should processed to decay lr",
                        default=350)

    parser.add_argument("--log-frequency", type=int,
                        help="How often to log results to the console",
                        default=10)

    parser.add_argument("--save-checkpoint-steps", type=int,
                        help="How often to save checkpoint",
                        default=1000)

    parser.add_argument("--save-summary-steps", type=int,
                        help="How often to save summary",
                        default=100)

    parser.add_argument("--filters-counts", nargs="+", type=int,
                        help="List of filter counts for each conv layer",
                        default=[96, 64])

    parser.add_argument("--conv-ksizes", nargs="+", type=int,
                        help="List of kernel sizes for each conv layer",
                        default=[10, 5])

    parser.add_argument("--conv-strides", nargs="+", type=int,
                        help="List of strides for each conv layer",
                        default=[])

    parser.add_argument("--pool-ksizes", nargs="+", type=int,
                        help="List of kernel sizes for each pool layer",
                        default=[3])

    parser.add_argument("--pool-strides", nargs="+", type=int,
                        help="List of strides for each pool layer",
                        default=[2])

    parser.add_argument("--fc-sizes", nargs="+", type=int,
                        help="List of sizes for each fc layer",
                        default=[2048, 2048, CAPTCHA_SIZE * len(ALPHABET)])

    parser.add_argument("--drop-rates", nargs="+", type=int,
                        help="List of probs for each conv and fc layer",
                        default=[])

    parser.add_argument("--data-format",
                        help="Data format: NCHW or NHWC",
                        default="NHWC")

    app_args = parser.parse_args()

    if tf.gfile.Exists(app_args.log_dir):
        tf.gfile.DeleteRecursively(app_args.log_dir)
    tf.gfile.MakeDirs(app_args.log_dir)
    tf.logging.set_verbosity("DEBUG")
    train(app_args)

