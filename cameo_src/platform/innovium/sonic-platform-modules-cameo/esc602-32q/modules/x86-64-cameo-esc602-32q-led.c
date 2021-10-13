/* An hwmon driver for Cameo esc602-32Q Innovium i2c Module */
#pragma GCC diagnostic ignored "-Wformat-zero-length"
#include "x86-64-cameo-esc602-32q.h"
#include "x86-64-cameo-esc602-32q-common.h"
#include "x86-64-cameo-esc602-32q-led.h"

/* i2c_client Declaration */
extern struct i2c_client *Cameo_CPLD_30_client; //0x30 for SYS CPLD
/* end of i2c_client Declaration */

/* implement i2c_function */
ssize_t led_ctrl_get(struct device *dev, struct device_attribute *da, char *buf)
{
    int status = -EPERM;
    int res = 0x1;
    int i;
    int led_a_status = 0;
    int led_g_status = 0;
    int led_b_status = 0;
    struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
    
    status = i2c_smbus_read_byte_data(Cameo_CPLD_30_client, LED_CTRL_REG);
    sprintf(buf, "");
    
    for (i = 1; i <= 4; i++)
    {
        if ( i == attr->index)
        {
            if (status & res)
            {
                led_a_status = ENABLE;
            }
            else
            {
                led_a_status = DISABLE;
            }
        }
        res = res << 1;
        if( i == (attr->index + 1) )
        {
            if (status & res)
            {
                led_g_status = ENABLE;
            }
            else
            {
                led_g_status = DISABLE;
            }
        }
        res = res << 1;
    }
    res = 0x1;

    status = i2c_smbus_read_byte_data(Cameo_CPLD_30_client, LED_BLINK_REG); 
    for (i = 1; i <= 4; i++)
    {
        if ( i == attr->index)
        {
            if (status & res)
            {
                led_b_status = ENABLE;
            }
            else
            {
                led_b_status = DISABLE;
            }
        }
        res = res << 1;
    }
    if(led_a_status == ENABLE && led_b_status == ENABLE)
    {
        sprintf(buf, "%s2\n", buf);
    }
    else if(led_a_status == ENABLE && led_b_status == DISABLE)
    {
        sprintf(buf, "%s1\n", buf);
    }
    else if(led_g_status == ENABLE && led_b_status == ENABLE)
    {
        sprintf(buf, "%s4\n", buf);
    }
    else if(led_g_status == ENABLE && led_b_status == DISABLE)
    {
        sprintf(buf, "%s3\n", buf);
    }
    else
    {
        sprintf(buf, "%s0\n", buf);
    }
    
    return sprintf(buf, "%s", buf);
}

ssize_t led_ctrl_set(struct device *dev, struct device_attribute *da, const char *buf, size_t count)
{
    int led_value  = -EPERM;
    int blk_value  = -EPERM;
    int result = -EPERM;
    int offset = 0;
    u16 i;
    struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	struct Cameo_i2c_data *data = i2c_get_clientdata(Cameo_CPLD_30_client);
    
    mutex_lock(&data->update_lock);
    led_value = i2c_smbus_read_byte_data(Cameo_CPLD_30_client, LED_CTRL_REG);
    blk_value = i2c_smbus_read_byte_data(Cameo_CPLD_30_client, LED_BLINK_REG);
    if (attr->index != 0)
    {
        i = simple_strtol(buf, NULL, 10);
        if(attr->index == 1)
        {
            offset = 0;
        }
        else
        {
            offset = 2*((attr->index)-1);
        }
        switch(i)
        {
            case SWITCH_LED_OFF: //i=0
                led_value   &= ~(0x03 << offset);
                blk_value   &= ~(1 << ((attr->index)-1));
                break;
            case SWITCH_LED_A_N: //i=1
                led_value   &= ~(0x03 << offset);
                led_value   |= (0x01 << offset);
                blk_value   &= ~(1 << ((attr->index)-1));
                break;
            case SWITCH_LED_A_B: //i=2
                led_value   &= ~(0x03 << offset);
                led_value   |= (0x01 << offset);
                blk_value   |= (1 << ((attr->index)-1));
                break;
            case SWITCH_LED_G_N: //i=3
                led_value   &= ~(0x03 << offset);
                led_value   |= (0x02 << offset);
                blk_value   &= ~(1 << ((attr->index)-1));
                break;
            case SWITCH_LED_G_B: //i=4
                led_value   &= ~(0x03 << offset);
                led_value   |= (0x02 << offset);
                blk_value   |= (1 << ((attr->index)-1));
                break;
            default:
                printk(KERN_ALERT "led_ctrl_set wrong Value\n");
                return count;
        }
        result  = i2c_smbus_write_byte_data(Cameo_CPLD_30_client, LED_CTRL_REG, led_value);
        result |= i2c_smbus_write_byte_data(Cameo_CPLD_30_client, LED_BLINK_REG, blk_value);
        if (result < 0)
        {
            printk(KERN_ALERT "ERROR: led_ctrl_set FAILED!\n");
        }
    }
    mutex_unlock(&data->update_lock);
    return count;
}

ssize_t led_fiber_get(struct device *dev, struct device_attribute *da, char *buf)
{
    struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	struct Cameo_i2c_data *data = i2c_get_clientdata(Cameo_CPLD_30_client);
    
    mutex_lock(&data->update_lock);
    sprintf(buf, "");
    if (attr->index == LED_FIBER)
    {
        if (i2c_smbus_read_byte_data(Cameo_CPLD_30_client, LED_FIBER_REG) & BIT_0_MASK)
        {
            sprintf(buf, "%s%d\n", buf, ENABLE);
        }
        else
        {
            sprintf(buf, "%s%d\n", buf, DISABLE);
        }
    }
    mutex_unlock(&data->update_lock);
    return sprintf(buf, "%s\n", buf);
}
ssize_t led_fiber_set(struct device *dev, struct device_attribute *da, const char *buf, size_t count)
{
    int status = -EPERM;
    int value  = -EPERM;
    int result = -EPERM;
    int input;
    struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	struct Cameo_i2c_data *data = i2c_get_clientdata(Cameo_CPLD_30_client);
    
    mutex_lock(&data->update_lock);
    status = i2c_smbus_read_byte_data(Cameo_CPLD_30_client, LED_FIBER_REG);
    if (attr->index == LED_FIBER)
    {
        input = simple_strtol(buf, NULL, 10);
        if (input == ENABLE)
        {
            value = status | LED_FIBER_ENABLE;
            result = i2c_smbus_write_byte_data(Cameo_CPLD_30_client, LED_FIBER_REG, value);
            if (result < 0)
            {
                printk(KERN_ALERT "ERROR: led_ctrl_set FAILED!\n");
            }
        }
        else if (input == DISABLE)
        {
            value = status & LED_FIBER_DISABLE;
            result = i2c_smbus_write_byte_data(Cameo_CPLD_30_client, LED_FIBER_REG, value);
            if (result < 0)
            {
                printk(KERN_ALERT "ERROR: led_ctrl_set FAILED!\n");
            }
        }
        else
        {
            printk(KERN_ALERT "led_ctrl_set wrong Value\n");
        }
    }
    mutex_unlock(&data->update_lock);
    return count;
}

ssize_t led_fiber_shift_freq_get(struct device *dev, struct device_attribute *da, char *buf)
{
    struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	struct Cameo_i2c_data *data = i2c_get_clientdata(Cameo_CPLD_30_client);
    int status = -EPERM;
    
    mutex_lock(&data->update_lock);
    sprintf(buf, "");
    if (attr->index == LED_FIBER_SHIFT_FREQ)
    {   
        // check CPLD FW version
        status = i2c_smbus_read_byte_data(Cameo_CPLD_30_client, 0x20);
        if(status < 0)
        {
            mutex_unlock(&data->update_lock);
            return status;
        }
        else if (status < 0xe)
        {
            mutex_unlock(&data->update_lock);
            return sprintf(buf, "%sunsupported\n\n", buf);
        }
        
        status = i2c_smbus_read_byte_data(Cameo_CPLD_30_client, LED_FIBER_SHIFT_FREQ_REG);
        if (status < 0)
        {
            mutex_unlock(&data->update_lock);
            return status;
        }
        
        switch(status)
        {
            case LED_FIBER_SHIFT_FREQ_378K:
                sprintf(buf, "%s378KHz\n", buf);
                break;
            case LED_FIBER_SHIFT_FREQ_576K:
                sprintf(buf, "%s576KHz\n", buf);
                break;
            case LED_FIBER_SHIFT_FREQ_1500K:
                sprintf(buf, "%s1.5MHz\n", buf);
                break;
            case LED_FIBER_SHIFT_FREQ_3000K:
                sprintf(buf, "%s3MHz\n", buf);
                break;
            default:
                sprintf(buf, "%sunknown\n", buf);
        }
    }
    mutex_unlock(&data->update_lock);
    return sprintf(buf, "%s\n", buf);
}

ssize_t led_fiber_shift_freq_set(struct device *dev, struct device_attribute *da, const char *buf, size_t count)
{
    struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	struct Cameo_i2c_data *data = i2c_get_clientdata(Cameo_CPLD_30_client);
    int status = -EPERM;
    int input;
    unsigned char freq = 0x00;
    
    mutex_lock(&data->update_lock);
    if (attr->index == LED_FIBER_SHIFT_FREQ)
    {
        // check CPLD FW version
        status = i2c_smbus_read_byte_data(Cameo_CPLD_30_client, 0x20);
        if(status < 0)
        {
            mutex_unlock(&data->update_lock);
            return status;
        }
        else if (status < 0xe)
        {
            mutex_unlock(&data->update_lock);
            printk(KERN_ALERT "unsupported\n");
            return -EPERM;
        }
        
        input = simple_strtol(buf, NULL, 10);
        
        switch(input)
        {
            case 378000:
                freq = LED_FIBER_SHIFT_FREQ_378K;
                break;
            case 576000:
                freq = LED_FIBER_SHIFT_FREQ_576K;
                break;
            case 1500000:
                freq = LED_FIBER_SHIFT_FREQ_1500K;
                break;
            case 3000000:
                freq = LED_FIBER_SHIFT_FREQ_3000K;
                break;
            default:
                printk(KERN_ALERT "unsupported freq\n");
                mutex_unlock(&data->update_lock);
                return -EPERM;
        }
        
        status = i2c_smbus_write_byte_data(Cameo_CPLD_30_client, LED_FIBER_SHIFT_FREQ_REG, freq);
        if (status < 0)
        {
            printk(KERN_ALERT "ERROR: led_fiber_shift_freq set FAILED!\n");
            mutex_unlock(&data->update_lock);
            return status;
        }
    }
    mutex_unlock(&data->update_lock);
    return count;
}

/* end of implement i2c_function */