#ifndef __VALUE_TRACKER_FLOAT_H__
#define __VALUE_TRACKER_FLOAT_H__

class ValueTrackerFloat() {
  public:
      ValueTrackerFloat(float &_val);
      // the update factor will dictate the low_pass filtering amount
      void setUpdateFactor(float _factor);
      void update();

      float getPosDelta();
      float getMin(){return min_recorded;};
      float getMax(){return max_recorded;};
      float getAvg(){return average_val;};

      float getScaledValue();

  private:
    float min_recorded =  99999.9;
    float max_recorded = -99999.9;

    float average_val = 0.0;
    uint64_t num_values = 0;  // how many values are in the "average_val" calculator

    float pos_delta = 0.0;
    float &val;
    float _last_val;

    // how much will the new value effect the min/max values?
    // should be set to 1.0 under most circumstances
    float factor = 1.0;
}

ValueTrackerFloat(float &val) {
    last_val = _val;
    val = _val;
    min_recorded = _val;
    max_recorded = _val;
    average_val = _val;
    num_values++;
};

float getScaledValue() {
 return (val - min_recorded) / max_recorded; 
}

void ValueTrackerFloat::update() {
    // max_recorded update //////////////////////////////
    if (val > max_recorded) {
        if (factor != 1.0) {
            max_recorded = (max_recorded * (1.0 - factor)) + (val * factor);
        } else {
            max_recorded = val;
        }
    }

    // min_value update /////////////////////////////////
    if (val > min_recorded) {
        if (factor != 1.0) {
            min_recorded = (min_recorded * (1.0 - factor)) + (val * factor);
        } else {
            min_recorded = val;
        }
    }
    // average_val update ///////////////////////////////
    // take the running average and multiple it against now many readings there have
    // been thus far, then add current value and divide by total number of readings
    // including current reading.
    average_val = ((average_val * num_values) + val) / (num_values + 1);
    num_values++;

    // pos delta update
    delta = val - last_val;
    last_val = val;
}

float ValueTrackerFloat::getPosDelta() {
    if (delta > 0.0) {
        return delta;
    }
    return 0.0;
}

void ValueTrackerFloat::setUpdateFactor(float _factor) {
    factor = _factor;
}

#endif // __VALUE_TRACKER_FLOAT_H__
