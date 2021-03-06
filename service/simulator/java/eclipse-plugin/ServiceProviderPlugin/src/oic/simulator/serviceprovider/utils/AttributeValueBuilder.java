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

package oic.simulator.serviceprovider.utils;

import java.util.Vector;

import org.oic.simulator.AttributeValue;

public class AttributeValueBuilder {
    public static AttributeValue build(String valueString,
            AttributeValue.ValueType valueType) {
        int depth = findDepth(valueString);
        if (0 == depth) {
            return handleDepth0(valueString, valueType);
        } else if (1 == depth) {
            return handleDepth1(valueString, valueType);
        } else if (2 == depth) {
            return handleDepth2(valueString, valueType);
        } else if (3 == depth) {
            return handleDepth3(valueString, valueType);
        }

        return null;
    }

    private static int findDepth(String value) {
        int depth = 0;
        for (char ch : value.toCharArray()) {
            if (ch == '[')
                depth++;
            else
                break;
        }

        return depth;
    }

    private static boolean isValidSyntax(String value) {
        int count = 0;
        for (char ch : value.toCharArray()) {
            if (ch == '[')
                count++;
            if (ch == ']')
                count--;
        }

        if (count == 0)
            return true;
        return false;

    }

    private static AttributeValue handleDepth0(String valueString,
            AttributeValue.ValueType valueType) {
        valueString = valueString.trim();
        if (0 != findDepth(valueString))
            return null;

        try {
            if (valueType == AttributeValue.ValueType.INTEGER)
                return new AttributeValue(Integer.parseInt(valueString));
            else if (valueType == AttributeValue.ValueType.DOUBLE) {
                Double value = Double.parseDouble(valueString);
                if (!value.isInfinite()) {
                    return new AttributeValue(value);
                }
            } else if (valueType == AttributeValue.ValueType.BOOLEAN) {
                if (valueString.equalsIgnoreCase("true")
                        || valueString.equalsIgnoreCase("false"))
                    return new AttributeValue(Boolean.parseBoolean(valueString));
            } else if (valueType == AttributeValue.ValueType.STRING)
                return new AttributeValue(valueString);
        } catch (Exception e) {
            return null;
        }
        return null;
    }

    private static String[] splitIntoArrays(String value) {
        Vector<String> values = new Vector<String>();
        String valueString = new String(value);
        valueString = valueString.substring(valueString.indexOf('[') + 1,
                valueString.lastIndexOf(']'));

        int count = 0;
        int startPos = 0;
        char[] charArray = valueString.toCharArray();
        for (int index = 0; index < charArray.length; index++) {
            if (charArray[index] == '[' && 0 == count++) {
                startPos = index;
            }

            if (charArray[index] == ']' && 0 == --count) {
                values.add(valueString.substring(startPos, index + 1));
            }
        }

        String[] result = new String[values.size()];
        values.toArray(result);
        return result;
    }

    private static AttributeValue handleDepth1(String valueString,
            AttributeValue.ValueType valueType) {
        valueString = valueString.trim();
        if (1 != findDepth(valueString) || false == isValidSyntax(valueString))
            return null;

        valueString = valueString.substring(valueString.indexOf('[') + 1,
                valueString.lastIndexOf(']'));
        String[] valuesString = valueString.split(",");
        if (null == valuesString || 0 == valuesString.length)
            return null;

        if (valueType == AttributeValue.ValueType.INTEGER) {
            Integer[] result = new Integer[valuesString.length];
            for (int index = 0; index < valuesString.length; index++) {
                Integer value = (Integer) handleDepth0(valuesString[index],
                        valueType).get();
                if (null == value)
                    return null;
                result[index] = value;
            }
            return new AttributeValue(result);
        } else if (valueType == AttributeValue.ValueType.DOUBLE) {
            Double[] result = new Double[valuesString.length];
            for (int index = 0; index < valuesString.length; index++) {
                Double value = (Double) handleDepth0(valuesString[index],
                        valueType).get();
                if (null == value)
                    return null;
                result[index] = value;
            }
            return new AttributeValue(result);
        } else if (valueType == AttributeValue.ValueType.BOOLEAN) {
            Boolean[] result = new Boolean[valuesString.length];
            for (int index = 0; index < valuesString.length; index++) {
                Boolean value = (Boolean) handleDepth0(valuesString[index],
                        valueType).get();
                if (null == value)
                    return null;
                result[index] = value;
            }
            return new AttributeValue(result);
        } else if (valueType == AttributeValue.ValueType.STRING) {
            return new AttributeValue(valuesString);
        }

        return null;
    }

    private static AttributeValue handleDepth2(String valueString,
            AttributeValue.ValueType valueType) {
        valueString = valueString.trim();
        if (2 != findDepth(valueString) || false == isValidSyntax(valueString))
            return null;

        String[] valuesString = splitIntoArrays(valueString);
        if (null == valuesString || 0 == valuesString.length)
            return null;

        if (valueType == AttributeValue.ValueType.INTEGER) {
            Integer[][] result = new Integer[valuesString.length][];
            for (int index = 0; index < valuesString.length; index++) {
                Integer[] value = (Integer[]) handleDepth1(valuesString[index],
                        valueType).get();
                if (null == value)
                    return null;
                result[index] = value;
            }
            return new AttributeValue(result);
        } else if (valueType == AttributeValue.ValueType.DOUBLE) {
            Double[][] result = new Double[valuesString.length][];
            for (int index = 0; index < valuesString.length; index++) {
                Double[] value = (Double[]) handleDepth1(valuesString[index],
                        valueType).get();
                if (null == value)
                    return null;
                result[index] = value;
            }
            return new AttributeValue(result);
        } else if (valueType == AttributeValue.ValueType.BOOLEAN) {
            Boolean[][] result = new Boolean[valuesString.length][];
            for (int index = 0; index < valuesString.length; index++) {
                Boolean[] value = (Boolean[]) handleDepth1(valuesString[index],
                        valueType).get();
                if (null == value)
                    return null;
                result[index] = value;
            }
            return new AttributeValue(result);
        } else if (valueType == AttributeValue.ValueType.STRING) {
            String[][] result = new String[valuesString.length][];
            for (int index = 0; index < valuesString.length; index++) {
                String[] value = (String[]) handleDepth1(valuesString[index],
                        valueType).get();
                if (null == value)
                    return null;
                result[index] = value;
            }
            return new AttributeValue(result);
        }

        return null;
    }

    public static AttributeValue handleDepth3(String valueString,
            AttributeValue.ValueType valueType) {
        valueString = valueString.trim();
        if (3 != findDepth(valueString) || false == isValidSyntax(valueString))
            return null;

        String[] valuesString = splitIntoArrays(valueString);
        if (null == valuesString || 0 == valuesString.length)
            return null;

        if (valueType == AttributeValue.ValueType.INTEGER) {
            Integer[][][] result = new Integer[valuesString.length][][];
            for (int index = 0; index < valuesString.length; index++) {
                Integer[][] value = (Integer[][]) handleDepth2(
                        valuesString[index], valueType).get();
                if (null == value)
                    return null;
                result[index] = value;
            }
            return new AttributeValue(result);
        } else if (valueType == AttributeValue.ValueType.DOUBLE) {
            Double[][][] result = new Double[valuesString.length][][];
            for (int index = 0; index < valuesString.length; index++) {
                Double[][] value = (Double[][]) handleDepth2(
                        valuesString[index], valueType).get();
                if (null == value)
                    return null;
                result[index] = value;
            }
            return new AttributeValue(result);
        } else if (valueType == AttributeValue.ValueType.BOOLEAN) {
            Boolean[][][] result = new Boolean[valuesString.length][][];
            for (int index = 0; index < valuesString.length; index++) {
                Boolean[][] value = (Boolean[][]) handleDepth2(
                        valuesString[index], valueType).get();
                if (null == value)
                    return null;
                result[index] = value;
            }
            return new AttributeValue(result);
        } else if (valueType == AttributeValue.ValueType.STRING) {
            String[][][] result = new String[valuesString.length][][];
            for (int index = 0; index < valuesString.length; index++) {
                String[][] value = (String[][]) handleDepth2(
                        valuesString[index], valueType).get();
                if (null == value)
                    return null;
                result[index] = value;
            }
            return new AttributeValue(result);
        }

        return null;
    }
}
