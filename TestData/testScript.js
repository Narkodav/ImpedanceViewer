function calculate(series1, series2) {   
    if(series1.length > series2.length) {
        return calculate(series2, series1);
    }

    // Simple test function that calculates average difference
    let sum = 0;
    let count = 0;
    
    for (; count < series1.length; count++) {
        sum += Math.abs(series1[count].y - series2[count].y);
    }
    
    return count > 0 ? sum / count : 0;
}