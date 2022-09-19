#ifndef __VALUE_TRACKER_DOUBLE_H__
#define __VALUE_TRACKER_DOUBLE_H__

#include <String.h>
#include "../PrintUtils/PrintUtils.h"

#ifndef LOG_SCALING
#define LOG_SCALING 0
#endif

#ifndef LINEAR_SCALING
#define LINEAR_SCALING 1
#endif

#ifndef EXP_SCALING
#define EXP_SCALING 2
#endif

// TODO - need to make it a child parent thing
class ValueTrackerDouble {
  public:
      ValueTrackerDouble(String n, double *_val, float lp);
      ValueTrackerDouble(String n, double *_val, float d_rate, uint16_t d_delay, float lp);
      // the update factor will dictate the low_pass filtering amount
      void update();
      void update(double v);
      void reset();

      double getPosDelta();
      double getNegDelta();
      double getDelta(){return delta;};


      double getMin(bool reset);
      double getMin(){return getMin(false);};

      double getMax(bool reset);
      double getMax(){return getMax(false);};

      void resetMinMax() {getMin(true);getMax(true);};
      void setMinMaxUpdateFactor(float mi, float ma);

      double getAvg(bool reset);
      double getAvg(){return getAvg(false);};

      double getRAvg(bool reset);
      double getRAvg(){return getRAvg(false);};

      double getLastVal(){return last_val;};

      ////////////////// scaling ////////////////
      double getScaled();
      double getScaledAvg();
      void setLinearScaling();
      void setLogScaling();
      void setExpScaling();

      void printStats();
      void print(){printStats();};

      void setPrintDecay(bool p){print_decay = p;};

      //////////////// utility functions ///////////////
      double calculateDelta(double v, double last_v);
      double calculateRollingAverage(double v, double last_v);
      double calculateScaledValue(double v, double min, double max);

      String getName(){return name;};

    double min_recorded;
    double max_recorded;

  private:

    String name = "";
    double *val;
    double last_val;

    /////////// Min/Max //////////////////////////////////
    bool updateMinMax();
    bool decayMinMax();
    // how quickly to update the min and max values
    float min_update_factor = 1.0;
    float max_update_factor = 1.0;
    
    /////////// average //////////////////////////////////
    double ravg_val =                      0.0;
    double avg_val  =                      0.0;
    // how many values are in the "average_val" calculator
    double updateAverage();

    uint64_t num_avg_values =              0;
    // how much will the new value effect the min/max values?
    // should be set to 1.0 under most circumstances
    // how much will the new value effect the rolling average?
    // a 1.0 will result in no averaging
    double low_pass_factor =               0.25;

    /////////// Delta ////////////////////////////////////
    double delta =                         0.0;

    /////////// scaled ///////////////////////////////////
    double scaled_val;
    double last_scaled_val;
    uint8_t scaling_type = EXP_SCALING;

    /////////// min/max decay ////////////////////////////
    // for decaying the min and max values
    elapsedMillis decay_timer;
    uint16_t decay_delay = 5000;
    double   decay_rate  = 0.005;

    /////////////////// Printing /////////////////////////
    bool print_decay = false;

    double minf(double l, double h);
    double maxf(double l, double h);
};

double ValueTrackerDouble::minf(double l, double h) { 
    if (l > h) {
        return h;
    }
    return l;
}

double ValueTrackerDouble::maxf(double l, double h) { 
    if (l > h) {
        return l;
    }
    return h;
}

ValueTrackerDouble::ValueTrackerDouble(String n, double *_val, float lp) {
    name = n;
    last_val = *_val;
    val = _val;
    min_recorded = *_val;
    max_recorded = *_val;
    ravg_val = *_val;
    avg_val = *_val;
    num_avg_values++;
    low_pass_factor = lp;
}

ValueTrackerDouble::ValueTrackerDouble(String n, double *_val, float d_rate, uint16_t d_delay, float lp){
    name = n;
    decay_rate = d_rate;
    decay_delay = d_delay;
    min_recorded = *_val;
    max_recorded = *_val;
    val = _val;
    low_pass_factor = lp;
}

void ValueTrackerDouble::reset() {
    Serial.print("resetting the averages and min/max values for the ");
    Serial.print(name);
    Serial.println(" value tracker double");
    min_recorded = *val;
    max_recorded = *val;
    num_avg_values = 1;
    avg_val = *val;
    ravg_val = *val;
}

void ValueTrackerDouble::printStats() {
    Serial.print("---------- printing ");
    Serial.print(name);
    Serial.println(" value tracker stats ----------------");
    Serial.print("name    : ");Serial.println(name);
    Serial.print("value   : "); Serial.println(*val, 4);
    Serial.print("min/max : ");Serial.print(min_recorded, 4);
    Serial.print("\t");Serial.println(max_recorded, 4);
    Serial.print("avg/ravg: ");Serial.print(avg_val, 4); 
    Serial.print("\t/\t");Serial.println(ravg_val, 4);
    Serial.print("numvals : ");Serial.println((long)num_avg_values);
    Serial.print("delta   : ");Serial.println(delta, 4);
}

double ValueTrackerDouble::getScaled() {
 return scaled_val;
}

double ValueTrackerDouble::getScaledAvg() {
    if (max_recorded - min_recorded > 0.0 && ravg_val > min_recorded) {
        return (ravg_val - min_recorded) / (max_recorded - min_recorded);
    }
    return 0.0;
}

// TODO - big problem, I dont think that the value tracker should operate off pointers,
// there is no reason to, instead values should be passed into things and the utility 
// functions should be public.
bool ValueTrackerDouble::updateMinMax(){
    // max_recorded update //////////////////////////////
    if (*val > max_recorded) {
        if (max_update_factor != 1.0) {
            // TODO - i think i need a different value for the min/ax low_padd_factor
            // Serial.print("max_recorded changed from x -> x:\t");Serial.print(max_recorded);
            max_recorded = (max_recorded * (1.0 - max_update_factor)) + (*val * max_update_factor);
            // Serial.print("\t->\t");Serial.println(max_recorded);
            *val = max_recorded;
            scaled_val = 1.0;
        } else {
            max_recorded = *val;
            scaled_val = 1.0;
        }
        return true;
    }
    else if (*val < min_recorded) {
        // min_value update /////////////////////////////////
        if (min_update_factor != 1.0) {
            // Serial.print("min_recorded changed from x -> x:\t");Serial.print(min_recorded);
            min_recorded = (min_recorded * (1.0 - min_update_factor)) + (*val * min_update_factor);
            // Serial.print("\t->\t");Serial.println(min_recorded);
            *val = min_recorded;
            scaled_val = 0.0;
        } else {
            min_recorded = *val;
            scaled_val = 0.0;
        }
        return true;
    }
    return false;
}

double ValueTrackerDouble::calculateScaledValue(double v, double min, double max){
    double s = 0.0;
    switch (scaling_type) {
        case LINEAR_SCALING:
            s = (v - min) / max;
            break;
        case LOG_SCALING:
            s = (v - min) / max;
            s = log(s) + 1;
            break;
        case EXP_SCALING:
            s = (v - min) / max;
            s = s * s;
            break;
        default:
            Serial.println("WARNING - valueTrackerDouble does not have a valid scaling_type");
            break;
    }

    // clip the scaled value
    s = minf(maxf(s, 0.0), 1.0);

    // Serial.print("scaled value is : ");Serial.println(scaled_val);
    // Serial.print("min/max is :\t");Serial.print(min_recorded);Serial.print("\t");
    // Serial.println(max_recorded);
    return s;
}

// TODO - change the code for the class so that each feature can be turned on and off
//

void ValueTrackerDouble::update(double v) {
    update();
 Serial.println("WARNING  THIS FUNCTION IS NOT YET IMPLEMENTED"); 
}

void ValueTrackerDouble::update() {
    decayMinMax();
    /////////////// scaled val //////////////////////////
    last_scaled_val = scaled_val;
    scaled_val = calculateScaledValue(*val, min_recorded, max_recorded);
    updateMinMax();

    // average_val update ///////////////////////////////
    // take the running average and multiple it against now many readings there have
    // been thus far, then add current value and divide by total number of readings
    // including current reading.
    avg_val = updateAverage();

    ////////////// Rolling Average /////////////////////
    // the rolling average is simple low pass filter 
    ravg_val = calculateRollingAverage(scaled_val, last_scaled_val);

    /////////////// Delta //////////////////////////////
    delta = calculateDelta(*val, last_val);
    last_val = *val;
}

bool ValueTrackerDouble::decayMinMax() {
    if (decay_timer > decay_delay) {
        // decrease based on the difference between min and max
        if (max_recorded > min_recorded) {
            dprint(print_decay, "decaying values, the difference between values is: ");
            double decay_factor = (max_recorded - min_recorded) * decay_rate;
            dprintln(print_decay, decay_factor, 4);
            dprint(print_decay, "min/max recorded changed : "); 
            dprint(print_decay, min_recorded, 4);
            dprint(print_decay, "\t");
            dprintln(print_decay, max_recorded);
            max_recorded = max_recorded - (decay_factor);
            min_recorded = min_recorded + (decay_factor);
            dprint(print_decay, "                         : "); 
            dprint(print_decay, min_recorded);
            dprint(print_decay, "\t");
            dprintln(print_decay, max_recorded);
            decay_timer = 0;
            return true;
        } else {
            max_recorded = min_recorded + 1.0;
            dprintln(print_decay, "Not decaying values, max_recorded is not less than min_recorded");
        }
    }
    return false;
}

double ValueTrackerDouble::calculateDelta(double v, double last_v) {
    return v - last_v;
}

double ValueTrackerDouble::updateAverage() {
    avg_val = ((num_avg_values * avg_val) + *val) / (num_avg_values + 1);
    num_avg_values++;
    return avg_val;
}

double ValueTrackerDouble::calculateRollingAverage(double v, double last_v) {
    return (last_v * (1.0 - low_pass_factor)) + (v * low_pass_factor);
}

double ValueTrackerDouble::getMin(bool reset) {
    double m = min_recorded;
    if (reset == true) {
        min_recorded = *val;
    }
    return m;
}

double ValueTrackerDouble::getMax(bool reset) {
    double m = max_recorded;
    if (reset == true) {
        max_recorded = *val;
    }
    return m;
}

double ValueTrackerDouble::getAvg(bool reset) {
    double a = avg_val;
    if (reset == true) {
        avg_val = *val;
        num_avg_values = 1;
    }
    return a;
}

double ValueTrackerDouble::getRAvg(bool reset) {
    double a = ravg_val;
    if (reset == true) {
        ravg_val = *val;
    }
    return a;
}

double ValueTrackerDouble::getPosDelta() {
    if (delta > 0.0) {
       return delta;
    }
    return 0.0;
}

double ValueTrackerDouble::getNegDelta() {
    if (delta < 0.0) {
        return delta;
    }
    return 0.0;
}

void ValueTrackerDouble::setMinMaxUpdateFactor(float mi, float ma) {
    min_update_factor = mi;
    max_update_factor = ma;
}

#endif // __VALUE_TRACKER_DOUBLE_H__
