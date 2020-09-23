#ifndef __VALUE_TRACKER_FLOAT_H__
#define __VALUE_TRACKER_FLOAT_H__

class ValueTrackerFloat {
  public:
      ValueTrackerFloat(float *_val, float lp);
      // the update factor will dictate the low_pass filtering amount
      void setUpdateFactor(float _factor);
      void update();
      void reset();

      float getPosDelta();
      float getNegDelta();
      float getDelta(){return delta;};

      float getMin(bool reset);
      float getMin(){return getMin(false);};

      float getMax(bool reset);
      float getMax(){return getMax(false);};

      float getAvg(bool reset);
      float getAvg(){return getAvg(false);};

      float getRAvg(bool reset);
      float getRAvg(){return getRAvg(false);};

      float getLastVal(){return last_val;};

      float getScaled();

      void printStats();

  private:
    float min_recorded =                   99999.9;
    float max_recorded =                   -99999.9;

    /////////// average
    float ravg_val =                      0.0;
    float avg_val  =                      0.0;

    // how many values are in the "average_val" calculator
    uint64_t num_avg_values =              0;
    float delta =                         0.0;

    float *val;
    float last_val;

    // how much will the new value effect the min/max values?
    // should be set to 1.0 under most circumstances
    float ravg_factor =                   1.0;
    // how much will the new value effect the rolling average?
    // a 1.0 will result in no averaging
    float low_pass_factor =               0.25;
};

ValueTrackerFloat::ValueTrackerFloat(float *_val, float lp) {
    last_val = *_val;
    val = _val;
    min_recorded = *_val;
    max_recorded = *_val;
    ravg_val = *_val;
    avg_val = *_val;
    num_avg_values++;
    low_pass_factor = lp;
}

void ValueTrackerFloat::reset() {
    Serial.println("resetting the averages and min/max values for the value tracker float");
    min_recorded = *val;
    max_recorded = *val;
    num_avg_values = 1;
    avg_val = *val;
    ravg_val = *val;
}

void ValueTrackerFloat::printStats() {
    Serial.println("---------- printing value tracker values ----------------");
    Serial.print("value   : "); Serial.println(*val);
    Serial.print("min/max : ");Serial.print(min_recorded);
    Serial.print("\t");Serial.println(max_recorded);
    Serial.print("avg/ravg: ");Serial.print(avg_val); 
    Serial.print("\t");Serial.println(ravg_val);
    Serial.print("numvals : ");Serial.println((long)num_avg_values);
    Serial.print("delta   : ");Serial.println(delta);
}

float ValueTrackerFloat::getScaled() {
 return (*val - min_recorded) / max_recorded;
}

void ValueTrackerFloat::update() {
    // max_recorded update //////////////////////////////
    if (*val > max_recorded) {
        if (ravg_factor != 1.0) {
            max_recorded = (max_recorded * (1.0 - ravg_factor)) + (*val * ravg_factor);
        } else {
            max_recorded = *val;
        }
    }
    // min_value update /////////////////////////////////
    if (*val < min_recorded) {
        if (ravg_factor != 1.0) {
            min_recorded = (min_recorded * (1.0 - ravg_factor)) + (*val * ravg_factor);
        } else {
            min_recorded = *val;
        }
    }
    // average_val update ///////////////////////////////
    // take the running average and multiple it against now many readings there have
    // been thus far, then add current value and divide by total number of readings
    // including current reading.
    avg_val = ((num_avg_values * avg_val) + *val) / (num_avg_values + 1);
    num_avg_values++;
 
    ////////////// Rolling Average /////////////////////
    // the rolling average is simple low pass filter 
    ravg_val = (last_val * (1.0 - low_pass_factor)) + (*val * low_pass_factor);

    /////////////// Delta //////////////////////////////
    delta = *val - last_val;
    last_val = *val;
}

float ValueTrackerFloat::getMin(bool reset) {
    float m = min_recorded;
    if (reset == true) {
        min_recorded = *val;
    }
    return m;
}

float ValueTrackerFloat::getMax(bool reset) {
    float m = max_recorded;
    if (reset == true) {
        max_recorded = *val;
    }
    return m;
}

float ValueTrackerFloat::getAvg(bool reset) {
    float a = avg_val;
    if (reset == true) {
        avg_val = *val;
        num_avg_values = 1;
    }
    return a;
}

float ValueTrackerFloat::getRAvg(bool reset) {
    float a = ravg_val;
    if (reset == true) {
        ravg_val = *val;
    }
    return a;
}

float ValueTrackerFloat::getPosDelta() {
    if (delta > 0.0) {
        return delta;
    }
    return 0.0;
}

float ValueTrackerFloat::getNegDelta() {
    if (delta < 0.0) {
        return delta;
    }
    return 0.0;
}

void ValueTrackerFloat::setUpdateFactor(float _factor) {
    ravg_factor = _factor;
}

#endif // __VALUE_TRACKER_float_H__
