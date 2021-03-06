/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/* General Partial MAR File Staged Patch Apply Test */

function run_test() {
  if (!shouldRunServiceTest(false, true)) {
    return;
  }

  gStageUpdate = true;
  setupTestCommon();
  gTestFiles = gTestFilesPartialSuccess;
  gTestFiles[gTestFiles.length - 2].originalContents = null;
  gTestFiles[gTestFiles.length - 2].compareContents = "FromPartial\n";
  gTestFiles[gTestFiles.length - 2].comparePerms = 0o644;
  gTestDirs = gTestDirsPartialSuccess;
  setupUpdaterTest(FILE_PARTIAL_MAR);

// This is commented out on mozilla-esr38 since it doesn't have the test updater
//  createUpdaterINI(false);

  // For Mac OS X set the last modified time for the root directory to a date in
  // the past to test that the last modified time is updated on all updates since
  // the precomplete file in the root of the bundle is renamed, etc. (bug 600098).
  if (IS_MACOSX) {
    let now = Date.now();
    let yesterday = now - (1000 * 60 * 60 * 24);
    let applyToDir = getApplyDirFile();
    applyToDir.lastModifiedTime = yesterday;
  }

  setupAppFilesAsync();
}

function setupAppFilesFinished() {
  runUpdateUsingService(STATE_PENDING_SVC, STATE_APPLIED);
}

function checkUpdateFinished() {
  checkFilesAfterUpdateSuccess(getStageDirFile, true, false);
  checkUpdateLogContents(LOG_PARTIAL_SUCCESS);

  if (IS_WIN || IS_MACOSX) {
    let running = getPostUpdateFile(".running");
    logTestInfo("checking that the post update process running file doesn't " +
                "exist. Path: " + running.path);
    do_check_false(running.exists());
  }

  // Switch the application to the staged application that was updated.
  gStageUpdate = false;
  gSwitchApp = true;
  do_timeout(TEST_CHECK_TIMEOUT, function() {
    runUpdate(0, STATE_SUCCEEDED);
  });
}

/**
 * Checks if the post update binary was properly launched for the platforms that
 * support launching post update process.
 */
function checkUpdateApplied() {
// This is commented out on mozilla-esr38 since it doesn't have the test updater
//  if (IS_WIN || IS_MACOSX) {
//    gCheckFunc = finishCheckUpdateApplied;
//    checkPostUpdateAppLog();
//  } else {
//    finishCheckUpdateApplied();
//  }
  do_timeout(TEST_HELPER_TIMEOUT, finishCheckUpdateApplied);
}

/**
 * Checks if the update has finished and if it has finished performs checks for
 * the test.
 */
function finishCheckUpdateApplied() {
  if (IS_MACOSX) {
    logTestInfo("testing last modified time on the apply to directory has " +
                "changed after a successful update (bug 600098)");
    let now = Date.now();
    let applyToDir = getApplyDirFile();
    let timeDiff = Math.abs(applyToDir.lastModifiedTime - now);
    do_check_true(timeDiff < MAC_MAX_TIME_DIFFERENCE);
  }

// This is commented out on mozilla-esr38 since it doesn't have the test updater
//  if (IS_WIN || IS_MACOSX) {
//    let running = getPostUpdateFile(".running");
//    logTestInfo("checking that the post update process running file exists. " +
//                "Path: " + running.path);
//    do_check_true(running.exists());
//  }

  checkFilesAfterUpdateSuccess(getApplyDirFile, false, false);
  checkUpdateLogContents(LOG_PARTIAL_SUCCESS);
  checkCallbackAppLog();
}
