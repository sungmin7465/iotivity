/*
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.oic.simulator;

/**
 * Class for representing invalid arguments exception.
 */
@SuppressWarnings("serial")
public class InvalidArgsException extends SimulatorException {

    public InvalidArgsException(int code, String message) {
        super(code, message);
    }

    public InvalidArgsException(SimulatorResult code, String message) {
        super(code, message);
    }

    public InvalidArgsException(String message) {
        super(SimulatorResult.SIMULATOR_INVALID_PARAM, message);
    }
}
