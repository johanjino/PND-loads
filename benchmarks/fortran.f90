program simple_loop
    implicit none
    
    integer :: i, sum
    
    ! Initialize sum to zero
    sum = 0
    
    ! Loop from 1 to 10
    do i = 1, 10
        sum = sum + i
    end do
    
    ! Print the result
    print *, 'The sum of the first 10 integers is:', sum
    
end program simple_loop

